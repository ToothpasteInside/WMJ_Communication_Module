#include <iostream>
#include "transport_can.h"
#include <cstring>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>

#define TIMEOUT 10

wmj::transport_can::transport_can(std::string port_name, bool enable_canfd_)
{
    // create a socketfd
    if ( (this->sock = socket( PF_CAN, SOCK_RAW, CAN_RAW )) < 0 ){
        std::cerr << "Create Socket Error!!" << std::endl;
        exit(1) ;
    }

    strcpy(ifr.ifr_name, "can0") ;
    ifr.ifr_ifindex = if_nametoindex( ifr.ifr_name ) ;
    ioctl(this->sock, SIOCGIFINDEX, &ifr) ;
    addr.can_family = AF_CAN ;
    addr.can_ifindex = ifr.ifr_ifindex ;
    if( bind(this->sock, (sockaddr*)&addr, sizeof(addr)) < 0 ) {
        std::cerr << "Bind Error!!" << std::endl;
        exit(1) ;
    }
    int fdflags = fcntl(sock, F_GETFL, 0);
    if(fcntl(sock, F_SETFL, fdflags | O_NONBLOCK) < 0)
    {
        std::cerr << "Set Nonblock Error!!" << std::endl;
        exit(1);
    }
}

wmj::transport_can::transport_can(const transport_can& tp)
{
    this->addr          = tp.addr ;
    this->ifr           = tp.ifr ;
}

wmj::transport_can::~transport_can()
{
    //this->MainRun.join() ;
}

bool wmj::transport_can::send_a_frame(Buffer &data)
{
    can_mutex.lock() ;
    int required_mtu = CAN_MTU ;
    required_mtu = util.parse_canframe(data, this->send_frame) ;

    if ( write( this->sock, &send_frame, required_mtu ) != required_mtu ) {
        std::cerr << "Error Write" << std::endl ;
        //std::cout << send_frame.can_id << std::endl ;
        can_mutex.unlock() ;
        return false ;
    }
    // close(this->sock_send) ;
    can_mutex.unlock() ;
    return true ;
}

bool wmj::transport_can::read_a_frame()
{
    can_mutex.lock() ;
    clock_t clock_begin = clock() ;
    bool flag = true ;
    while(flag)
    {
        std::cout << "Begin read" << std::endl ;
        int nbytes = read(this->sock, &(this->read_frame), sizeof(this->read_frame)) ;
        std::cout << "reading" << std::endl ;
        if(nbytes < 0) {
            std::cerr << "Interface down" << std::endl ;
            flag = false ;
        }
        else if ( nbytes == CANFD_MTU || nbytes == CAN_MTU )
            break ;
        else 
            ;
        if( (double)(clock() - clock_begin)/CLOCKS_PER_SEC > (double)TIMEOUT/1000.0 )
            break ;
    }
    //std::cout << read_frame.data[2] << std::endl ;
    can_mutex.unlock() ;
    return flag ;
}

void wmj::transport_can::mainRun()
{
    std::cout << "thread start !" << std::endl ;
    while( !EOF ) {
        if( this->read_a_frame() )
            continue ;
        else break ;
    }
    std::cout << "thread stop !" << std::endl ;
}

/*int main()
{
    wmj::transport_can can{"can0", false};
    //int loop, delay ;
    int rcv_cnt{0}, send_cnt{0} ;
    //Buffer a{1, 2, 1, '#', 0xff, 0xff, 0xff, 0xff};
    Buffer a{'1', '2', '1', '#', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd'};
    while(1)
    {
        //std::cout << "One loop begin" << std::endl ;
        //can.send_a_frame(a) ;
        //usleep(10000) ;
        std::cout << rcv_cnt << " package received" << std::endl ;
        //std::cout << "One loop end" << std::endl ;
    }
    std::cout << "Please input the time to delay usleep" << std::endl ;
    std::cin >> delay ;
    std::cout << "Please input the number of loop" << std::endl ;
    std::cin >> loop ;
    for ( int i = 0; i < 10000; i++ )
    {
        //std::cout << "Loop" << i << std::endl ;
        if(rcv_cnt !=10000 && can.read_a_frame()) {
            if (can.read_frame.can_id == 289)
                rcv_cnt ++ ;
        }
        if(can.send_a_frame(a))
            send_cnt ++ ;
        usleep(500) ;
        std::cout << rcv_cnt << " package received" << std::endl ;
        std::cout << send_cnt << " package sent" << std::endl ;
    }
}*/
