#pragma onces

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
        static const char* GetVendor();
    };

    extern "C" const char* cpu_get_vendor();
}
