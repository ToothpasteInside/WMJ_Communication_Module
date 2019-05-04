#include "utils.h"
#define CANID_DELIM '#'

static const uint8_t dlc2len[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64} ;
static const uint8_t len2dlc[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,    // 0 - 8
                                9, 9, 9, 9,                     // 9 - 12
                                10, 10, 10, 10,                 // 13 - 16
                                11, 11, 11, 11,                 // 17 - 20
                                12, 12, 12, 12,                 // 21 - 24
                                13, 13, 13, 13, 13, 13, 13, 13, // 25 - 32
                                14, 14, 14, 14, 14, 14, 14, 14, // 33 - 40
                                14, 14, 14, 14, 14, 14, 14, 14, // 41 - 48
                                15, 15, 15, 15, 15, 15, 15, 15, // 49 - 56
                                15, 15, 15, 15, 15, 15, 15, 15, // 57 - 64
                                };

uint8_t wmj::utils::can_dlc2len( uint8_t can_dlc )
{
    return dlc2len[ can_dlc & 0x0f ] ;
}

uint8_t wmj::utils::can_len2dlc( uint8_t len)
{
    if ( len > 64 ) 
        return 0xf ;
    return len2dlc[len] ;
}

uint8_t wmj::utils::asc2nibble( uint8_t data )
{
    if ( data >= '0' && data <= '9' )
        return data - '0' ;
    if ( data >= 'A' && data <= 'F' )
        return data - 'A' + 10 ;
    if ( data >= 'a' && data <= 'f' )
        return data - 'a' + 10 ;
    else return 16 ;
}

/**
 * @brief 将上层数据包打包成CAN帧
 *
 * @param data
 * @param frame
 *
 * @return 
 */
int wmj::utils::parse_data(Buffer &data, canfd_frame &frame)
{
    int idx, len ;
    int ret         = CAN_MTU ;
    
    len             = data.size() ;
    memset( &frame, 0, sizeof(frame) ) ;

    if ( len < 2 )
        return 0 ;

    idx         = 1 ;
    switch(data[0])
    {
    case 0x01:
        frame.can_id = 0x301 ;
        break ;
    case 0x05:
        frame.can_id = 0x200 ;
        break ;
    case 0x03:
        frame.can_id = 0x401 ;
    default:
        frame.can_id = 0x100 ;
    }
    //std::cout << frame.can_id << std::endl ;

    for ( int i = 0; i < len - 1; i++ ) {
        frame.data[i] = data[idx++] ;
    }
    frame.len = len - 1 ;
    return ret ;
}

/**
 * @brief 将CAN帧解析为上层数据包
 *
 * @param data
 * @param frame
 *
 * @return 
 */
int wmj::utils::parse_canframe(Buffer &data, canfd_frame &frame)
{
    data.clear() ;
    switch(frame.can_id)
    {
    case 0x302:
        data.push_back(1) ;
        break ;
    case 0x402:
        data.push_back(3) ;
        break ;
    case 0x202:
        data.push_back(5);
        break;
    case 0x313:
        data.push_back(6);
        break;
    default :
        data.push_back(0) ;
    }
    for( auto c : frame.data )
    {
        data.push_back(c) ;
    }
    return data.size() ;
}
