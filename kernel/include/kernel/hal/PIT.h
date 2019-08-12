#ifndef _PIT_H_INCLUDED
#define _PIT_H_INCLUDED

#if !defined(__i386__)
#error "[pit.h] platform not implimented."
#endif

#include <stdint.h>

namespace kernel
{
    class PIT
    {
    public:
        enum class CommandMasks
        {
            BINCOUNT = 1,           //00000001
            MODE = 0xE,		        //00001110
            RL = 0x30,	            //00110000
            COUNTER = 0xC0	        //11000000
        };

        enum class BinaryCountMode
        {
            BINARY = 0,		        //0
            BCD = 1		            //1
        };

        enum class CounterMode
        {
            TERMINALCOUNT = 0,      //0000
            ONESHOT = 0x2,		    //0010
            RATEGEN = 0x4,		    //0100
            SQUAREWAVEGEN = 0x6,    //0110
            SOFTWARETRIG = 0x8,	    //1000
            HARDWARETRIG = 0xA 	    //1010
        };

        enum class TransferType
        {
           LATCH = 0,			    //000000
           LSBONLY = 0x10,	        //010000
           MSBONLY = 0x20,		    //100000
           DATA = 0x30,		        //110000
        };

        enum class Counter
        {
            COUNTER_0 = 0,	        //00000000
            COUNTER_1 = 0x40,	    //01000000
            COUNTER_2 = 0x80	    //10000000
        };

        static void SendCommand(uint8_t cmd);
        static void SendData(uint16_t data, uint8_t counter);
        static uint8_t ReadData(uint16_t counter);

        //Sets new pit tick count and returns previous value
        static uint32_t SetTickCount(uint32_t i);
        static uint32_t GetTickCount();

        //starts a counter. Counter continues until this method is called again
        static void StartCounter(uint32_t freq, uint8_t counter, uint8_t mode);

        //Initialise minidriver
        static void Initialise();
    };
    
    extern "C" void handle_irq();
}
#endif
