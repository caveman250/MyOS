#ifndef _HAL_H
#define _HAL_H

#if !defined(__i386__)
#pragma error "HAL not implimented for this platform"
#endif

#include <stdint.h>

namespace kernel
{
    class HAL
    {
    public:
        static void Initialise();
        static void Shutdown ();
    };
}

extern "C" void Geninterrupt ();

#endif
