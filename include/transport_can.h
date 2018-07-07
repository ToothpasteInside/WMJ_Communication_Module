#pragma once
#include "Transport.h"
#include <linux/can/raw.h>

#include <mutex>
#include <thread>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/if.h>

namespace wmj{
class transport_can : public Transport
{
public:
    transport_can(std::string, bool) ;
    transport_can(const transport_can&) ;
    virtual ~transport_can() ;
    virtual bool send_a_frame(Buffer&) ;
    virtual bool read_a_frame() ;
    void mainRun() ;
    void startThread() ;
    canfd_frame     send_frame{} ;
    canfd_frame     read_frame{} ;

private:
    int             sock{} ;
    sockaddr_can    addr{} ;
    iovec           iov{} ;
    utils           util ;
    ifreq           ifr{} ;
    std::mutex      can_mutex{} ;
    std::thread     MainRun{} ;
};
}
