#pragma once

#if !defined(__i386__)
#error "[pit.h] platform not implimented."
#endif

#include <stdint.h>

namespace kernel::hal
{
    class PIT
    {
    public:
        static PIT& Get() {return s_Instance; }

        void Initialise();
        void SendCommand(uint8_t cmd);
        void SendData(uint16_t data, uint8_t counter);
        uint8_t ReadData(uint16_t counter);
        
        uint32_t GetTickCount();
        void SetTickCount(uint32_t i);
        void IncrementTickCount();

        //starts a counter. Counter continues until this method is called again
        void StartCounter(uint32_t freq, uint8_t counter, uint8_t mode);

        enum class Counter
        {
            Counter0 = 0,           //00000000
            Counter1 = 0x40,        //01000000
            Counter2 = 0x80         //10000000
        };

        enum class CounterMode
        {
            TerminalCount = 0,          //0000
            OneShot = 0x2,		        //0010
            RateGen = 0x4,		        //0100
            SqureWaveGen = 0x6,         //0110
            SoftwareTrigger = 0x8,	    //1000
            HardwareTrigger = 0xA 	    //1010
        };
        
    private: 
        enum class CommandMasks
        {
            BinCount = 1,           //00000001
            Mode = 0xE,		        //00001110
            ReadLoad = 0x30,	            //00110000
            Counter = 0xC0	        //11000000
        };

        enum class BinaryCountMode
        {
            Binary = 0,		        //0
            BinaryCodedDecimal = 1  //1
        };

        enum class TransferType
        {
           Latch = 0,			                //000000
           LeastSignificantByteOnly = 0x10,	    //010000
           MostSignificantBitOnly = 0x20,		//100000
           Data = 0x30,		                    //110000
        };

        enum class ControlRegisters
        {
            Counter0 = 0x40,
            Counter1 = 0x41,
            Counter2 = 0x42,
            Command = 0x43
        };

        volatile uint32_t m_PitTicks;

        static PIT s_Instance;
    };
    
    extern "C" void handle_irq();
}
