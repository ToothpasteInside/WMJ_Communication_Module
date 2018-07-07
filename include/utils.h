#pragma once

#ifndef CAN_UTILS
#define CAN_UTILS

#include <iostream>
#include <linux/can.h>
#include <vector>
#include <cstring>

typedef std::vector<uint8_t> Buffer ;

/**
 * @brief can utils for transport_can
 * @author No one
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
    
    int parse_canframe(Buffer&, canfd_frame&) ;
private:

};
}

#endif
