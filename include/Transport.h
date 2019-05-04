#pragma once
#include <iostream>
#include <vector>

namespace wmj{
typedef std::vector<uint8_t> Buffer;
class Transport
{
public:
    Transport() {}
    virtual ~Transport() {}
    virtual bool send_a_frame(Buffer&) = 0 ;
    virtual Buffer read_a_frame(int) = 0 ;
private:
};
}

