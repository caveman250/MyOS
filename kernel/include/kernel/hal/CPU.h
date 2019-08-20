#pragma once

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel::hal
{
    class CPU
    {
    public:
        static CPU& Get() { return s_Instance; }
        void Initialise ();
        void Shutdown ();
        const char* GetVendor();

    private:
        static CPU s_Instance;
    };

    extern "C" const char* cpu_get_vendor();
}
