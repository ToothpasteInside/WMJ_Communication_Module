#pragma once
/* Copyright (C) 
 * 2018 - XiaMo Northwestern Polytechnical University
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "Transport.h"
#include "utils.h"
#include <linux/can/raw.h>

#include <mutex>
#include <thread>
#include <memory>
#include <queue>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/if.h>

namespace wmj{
class TransportCan : public Transport
{
public:
    TransportCan(std::string) ;
    TransportCan(const TransportCan&) ;
    virtual ~TransportCan() ;
    virtual bool send_a_frame(Buffer&) ;
    virtual Buffer read_a_frame(int) ;
    void startThread() ;
    void stopThread() ;

private:
    int             _sock{} ;
    sockaddr_can    _addr{} ;
    iovec           _iov{} ;
    ifreq           _ifr{} ;

    std::mutex      _can_mutex{} ;
    std::mutex      _read_buffer_mutex{} ;
    std::mutex      _write_buffer_mutex{} ;

    std::thread     _readThread{} ;
    std::thread     _writeThread{} ;

    canfd_frame     _send_frame{} ;
    canfd_frame     _read_frame{} ;

    Buffer          _read_maincontrl_buffer{} ;
    Buffer          _read_gimbal_buffer{} ;
    Buffer          _read_chassis_buffer{} ;
    Buffer          _read_gyro_buffer{} ;
    Buffer          _read_buffer{} ;

    std::queue<Buffer> _write_buffer{} ;
    bool            _thread_run{true} ;
    std::shared_ptr<utils> _util ;
    bool try_write() ;
    bool try_read() ;
    void readRun() ;
    void writeRun() ;
};
}
