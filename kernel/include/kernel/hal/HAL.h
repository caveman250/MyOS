#pragma once

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

        //output sound to speaker
        static void Sound(unsigned frequency);

        //read/write byte to/from device using port mapped io
        static unsigned char InB(unsigned short portid);
        static void OutB(unsigned short portid, unsigned char value);

        static void EnableInterrupts();
        static void DisableInterrupts();

        static void SetInterruptRoutine(int intno, uint32_t vect);
        static void	(*GetInterruptRoutine (int intno))();
        //notifies hal interrupt is finished
        static void InterruptFinished(unsigned int intno);

        static const char* GetCpuVendor();
        static int GetTickCount();
    };
}

extern "C" void hal_gen_interrupt();
