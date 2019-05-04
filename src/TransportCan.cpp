#include <iostream>
#include "TransportCan.h"
#include <cstring>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

constexpr double TIMEOUT = 2;


/**
 * @brief 根据设备名选择通信设备并启动读写线程
 *
 * @param port_name
 */
wmj::TransportCan::TransportCan(std::string port_name)
{
    _util          = std::make_shared<utils>() ;
    // create a socketfd
    if ( (_sock = socket( PF_CAN, SOCK_RAW, CAN_RAW )) < 0 ) {
        std::cerr << "Create Socket Error!!" << std::endl;
        exit(1) ;
    }

    strcpy(_ifr.ifr_name, port_name.c_str()) ;
    _ifr.ifr_ifindex     = if_nametoindex( _ifr.ifr_name ) ;
    if( ioctl(_sock, SIOCGIFINDEX, &_ifr) < 0 ) {
        std::cerr << "Set io error" << std::endl ;
        exit(1) ;
    }
    _addr.can_family     = AF_CAN ;
    _addr.can_ifindex    = _ifr.ifr_ifindex ;

    if( bind(_sock, (sockaddr*) & _addr, sizeof(_addr)) < 0 ) {
        std::cerr << "Bind Error!!" << std::endl;
        exit(1) ;
    }

    int fdflags = fcntl(_sock, F_GETFL, 0);

    if(fcntl(_sock, F_SETFL, fdflags | O_NONBLOCK) < 0)
    {
        std::cerr << "Set Nonblock Error!!" << std::endl;
        exit(1);
    }

    this->_read_gimbal_buffer.resize(255);
    this->_read_maincontrl_buffer.resize(255);
    this->_read_gyro_buffer.resize(255);
    //std::cerr << "Prepare to Start the thread" << std::endl ;
    this->startThread() ;
}

/**
 * @brief 启动读写线程
 */
void wmj::TransportCan::startThread()
{
    _thread_run = true ;
    this->_readThread = std::thread(&wmj::TransportCan::readRun, this) ;
    this->_readThread.detach() ;
    this->_writeThread = std::thread(&wmj::TransportCan::writeRun, this) ;
    this->_writeThread.detach() ;
}

void wmj::TransportCan::stopThread()
{
    _thread_run = false ;
}
/**
 * @brief 由于类成员中有不可拷贝的对象，所以覆盖掉默认的拷贝构造函数，以解决兼容性问题。
 *
 * @param tp
 */
wmj::TransportCan::TransportCan(const TransportCan& tp)
{
    _addr          = tp._addr ;
    _ifr           = tp._ifr ;
}

wmj::TransportCan::~TransportCan()
{
    this->stopThread() ;
}

/**
 * @brief 发送一帧数据（外部接口，由于是非阻塞的，所以）
 *
 * @param data
 *
 * @return 发送是否成功
 */
bool wmj::TransportCan::send_a_frame(Buffer &data)
{
    _write_buffer_mutex.lock() ;
    try{
        _write_buffer.push(data) ;
    }
    catch (std::exception &ex){
        std::cerr << ex.what() ;
        _write_buffer_mutex.unlock() ;
        return false ;
    }
    if(_write_buffer.size() > 5) {
        _write_buffer.pop() ;
    }
    _write_buffer_mutex.unlock() ;
    return true ;
}

/**
 * @brief 读取一帧数据（外部接口）
 *
 * @param flag 要读取的数据帧类型
 *
 * @return 原始数据帧
 */
wmj::Buffer wmj::TransportCan::read_a_frame(int flag)
{
    wmj::Buffer value;
    std::lock_guard<std::mutex> lock_guard(_read_buffer_mutex);
    switch(flag)
    {
    case 0:{
        value = Buffer{0};
        break;
    }
    case 1:{
        value = _read_gimbal_buffer;
        break;
    }
    case 5:{
        value = _read_maincontrl_buffer;
        break;
    }
    case 6:{
        value = _read_gyro_buffer;
        break;
    }
    default:{
        value = Buffer{0xff};
        break;
    }
    }
    return value;
}

bool wmj::TransportCan::try_write()
{
    std::lock_guard<std::mutex> lock_guard_can(_can_mutex);
    std::lock_guard<std::mutex> lock_guard_write(_write_buffer_mutex);
    int required_mtu{};
    if(!_write_buffer.empty())
    {
        required_mtu = _util->parse_data(_write_buffer.front(), this->_send_frame);    //parse data to can frame
        _write_buffer.pop();
    }
    else
    {
        _can_mutex.unlock() ;
        _write_buffer_mutex.unlock() ;
        return false;
    }

    if ( send( _sock, &_send_frame, required_mtu, MSG_DONTWAIT ) != required_mtu ) {
        //std::cerr << "Error Write" << std::endl;
        //std::cout << required_mtu << std::endl;
        //std::cout << write( _sock, &_send_frame, required_mtu );
        //std::cout << "Error Write And errorno is " << errno << std::endl;
        //std::cout << strerror(errno) << std::endl;
        if(errno == 100) {
            system("cd /home/nvidia && ./enCanBus.sh");
        }
        return false;
    }
    return true;
}
/**
 * @brief 尝试读取系统缓冲区
 *
 * @return 
 */
bool wmj::TransportCan::try_read()
{
    std::lock_guard<std::mutex> lock_guard_can(_can_mutex);
    clock_t clock_begin = clock() ;
    bool flag = true ;
    while(flag)
    {
        int nbytes = recv(_sock, &(_read_frame), sizeof(_read_frame), MSG_DONTWAIT) ;
        if(nbytes < 0) {
            if(errno == 11) {
                //std::cout << strerror(errno) << std::endl ;
            }
            if( errno == EAGAIN ) {
                //std::cerr << "No more data in buffer" << std::endl ;
            }
            flag = false;
        }
        else if ( nbytes == CAN_MTU )
        {
            std::lock_guard<std::mutex> lock_guard_read(_read_buffer_mutex);
            _util->parse_canframe(_read_buffer, _read_frame) ;
            switch(_read_buffer[0])
            {
            case 1: {
                std::copy(_read_buffer.begin()+1, _read_buffer.end(), _read_gimbal_buffer.begin()) ;
                break; 
            }
            case 5:{
                std::copy(_read_buffer.begin()+1, _read_buffer.end(), _read_maincontrl_buffer.begin()) ;
                break;
            }
            case 6:{
                std::copy(_read_buffer.begin()+1, _read_buffer.end(), _read_gyro_buffer.begin()) ;
                break;
            }
            default:
                break;
            }
            break ;
        }
        else {
            std::cerr << "Incomplete data packs" << std::endl ;
        }

        if( (double)(clock() - clock_begin)/CLOCKS_PER_SEC > (double)TIMEOUT/1000.0 )
        {
            std::cerr << "TimeOut!" << std::endl ;
            flag = false;
        }
    }
    return flag ;
}

/**
 * @brief 读线程--1ms间隔循环读取底层缓冲区
 */
void wmj::TransportCan::readRun()
{
    std::cout << "read thread start !" << std::endl ;
    while( _thread_run ) {
        this->try_read();
        usleep(500) ;
    }
    std::cout << "read thread stop !" << std::endl ;
    return ;
}

/**
 * @brief 写线程--1ms间隔将任务队列中的消息发出
 */
void wmj::TransportCan::writeRun()
{
    std::cout << "write thread start !" << std::endl ;
    while( _thread_run ) {
        this->try_write();
        usleep(200) ;
    }
    std::cout << "write thread stop !" << std::endl ;
    return ;
}
