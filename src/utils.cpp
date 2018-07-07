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

int wmj::utils::parse_canframe(Buffer &data, canfd_frame &frame)
{
    int idx, len ;
    int ret         = CAN_MTU ;
    uint8_t tmp ;
    
    len             = data.size() ;
    memset( &frame, 0, sizeof(frame) ) ;

    if ( len < 4 )
        return 0 ;

    idx         = 4 ;
    for ( int i = 0; i < 3; i++ ) {
        if ( ( tmp = this->asc2nibble(data[i]) ) > 0x0f )
            return 0 ;
        //std::cout << tmp << std::endl ;
        frame.can_id |= ( tmp << (2-i)*4 ) ;
    }
    //std::cout << frame.can_id << std::endl ;

    for ( int i = 0; i < len - 4; i++ ) {
        frame.data[i] = data[idx++] ;
    }
    frame.len = len - 4 ;
    return ret ;
}
