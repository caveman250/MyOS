#ifndef _CPU_H_INCLUDED
# define _CPU_H_INCLUDED

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel
{
    class CPU
    {
    public:
        static void Initialise ();
        static void Shutdown ();
    };
}

#endif
