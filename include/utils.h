/* Copyright (C) 
 * 2019 - Xiamo Northwestern Polytechnical University
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
#pragma once

#ifndef CAN_UTILS
#define CAN_UTILS

#include <iostream>
#include <linux/can.h>
#include <cstring>
#include "Transport.h"


/**
 * @brief can utils for transport_can
 * @author Xiamo
**/

namespace wmj{
class utils
{
public:
    utils() {}
    ~utils() {}
    /* get data length from can_dlc with sanitized can_dlc */
    uint8_t can_dlc2len(uint8_t) ;
    /* map the sanitized data length to an appropriate data length code */
    uint8_t can_len2dlc(uint8_t) ;
    /* parse ASCII hex character to dec number */
    uint8_t asc2nibble(uint8_t) ;
    
    int parse_data(Buffer&, canfd_frame&) ;
    int parse_canframe(Buffer&, canfd_frame&) ;
private:

};
}

#endif
