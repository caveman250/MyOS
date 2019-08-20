#pragma once

#if !defined(__i386__)
#pragma error "HAL not implimented for this platform"
#endif

#include <stdint.h>

namespace kernel::hal
{
    class HAL
    {
    public:
        static HAL& Get() { return s_Instance; }

        void Initialise();
        void Shutdown ();

        //output sound to speaker
        void Sound(unsigned frequency);

        //read/write byte to/from device using port mapped io
        unsigned char InB(unsigned short portid);
        void OutB(unsigned short portid, unsigned char value);

        void EnableInterrupts();
        void DisableInterrupts();

        void SetInterruptRoutine(int intno, uint32_t vect);
        void (*GetInterruptRoutine(int intno))();
        //notifies hal interrupt is finished
        void InterruptFinished(unsigned int intno);

        const char* GetCpuVendor();
        int GetTickCount();

    private:

        static HAL s_Instance;
    };
}

extern "C" void hal_gen_interrupt();
