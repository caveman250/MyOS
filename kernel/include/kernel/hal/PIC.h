#pragma once

#if !defined(__i386__)
#error "[pic.h] platform not implimented."
#endif

#include <stdint.h>

namespace kernel
{
    class PIC
    {
    public:
        //The following devices use PIC 1 to generate interrupts
        enum class PIC1Device
        {
            TIMER = 0,
            KEYBOARD = 1,
            SERIAL2 = 3,
            SERIAL1 = 4,
            PARALLEL2 = 5,
            DISKETTE = 6,
            PARALLEL1 = 7
        };

        //The following devices use PIC 2 to generate interrupts
        enum class PIC2Device
        {
            CMOSTIMER = 0,
            CGARETRACE = 1,
            AUXILIARY = 4,
            FPU = 5,
            HDC = 6
        };

        //Command Word 2 bit masks. Used for sending commands
        enum class CommandWord2Mask
        {
            L1 = 1,           //00000001
            L2 = 2,           //00000010
            L3 = 4,           //00000100
            EOI = 0x20,       //00100000
            SL = 0x40,        //01000000
            ROTATE = 0x80     //10000000
        };

        //Command Word 3 bit masks. Used for sending commands
        enum class CommandWord3Mask
        {
            RIS = 1,          //00000001
            RIR = 2,          //00000010
            MODE = 4,         //00000100
            SMM = 0x20,       //00100000
            ESMM = 0x40,      //01000000
            D7 = 0x80         //10000000
        };

        static uint8_t
        ReadData(uint8_t picNum);
        static void SendData(uint8_t data, uint8_t picNum);
        static void SendCommand(uint8_t cmd, uint8_t picNum);

        static void Initialise(uint8_t base0, uint8_t base1);
    };
}
