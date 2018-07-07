#pragma once
#include "utils.h"

namespace wmj{
class Transport
{
public:
    Transport() {}
    virtual ~Transport() {}
    virtual bool send_a_frame(Buffer&) = 0 ;
    virtual bool read_a_frame() = 0 ;
private:
};
}

