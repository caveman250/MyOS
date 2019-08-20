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
        //Command Word 2 bit masks. Used for sending commands
        enum class CommandWord2Mask
        {
            InterruptLevel1 = 1,        //00000001
            InterruptLevel2 = 2,        //00000010
            InterruptLevel3 = 4,        //00000100
            EndOfInterrupt = 0x20,      //00100000
            Selection = 0x40,           //01000000
            Rotate = 0x80               //10000000
        };

        static PIC& Get() { return s_Instance; }

        void Initialise(uint8_t base0, uint8_t base1);

        uint8_t ReadData(uint8_t picNum);
        void SendData(uint8_t data, uint8_t picNum);
        void SendCommand(uint8_t cmd, uint8_t picNum);

private:
        //The following devices use PIC 1 to generate interrupts
        enum class PIC1Device
        {
            Timer = 0,
            Keyboard = 1,
            Serial2 = 3,
            Serial1 = 4,
            Parallel2 = 5,
            Diskette = 6,
            Parallel1 = 7
        };

        //The following devices use PIC 2 to generate interrupts
        enum class PIC2Device
        {
            CmosTimer = 0,
            CgareTrace = 1,
            Auxiliary = 4,
            Fpu = 5,
            Hdc = 6
        };

        //PIC 1 register port addresses
        enum class PIC1PortAddress
        {
            Command = 0x20,
            Status	= 0x20,
            Data = 0x21,
            InterruptMaskRegister = 0x21
        };

        //PIC 2 register port addresses
        enum class PIC2PortAddress
        {
            Command = 0xA0,
            Status	= 0xA0,
            Data = 0xA1,
            InterruptMaskRegister = 0xA1
        };

        //Initialisation Control Word 1 bit masks
        enum class InitialisationWord1Mask
        {
            IC4 = 0x1,			        //00000001
            Single = 0x2,			    //00000010
            AdressInterval = 0x4,		//00000100
            LevelTriggeredMode = 0x8,	//00001000
            Initialise = 0x10		    //00010000
        };

        //Initialisation Control Words 2 and 3 do not require bit masks

        //Initialisation Control Word 4 bit masks
        enum class InitialisationWord4Mask
        {
            uPM	= 0x1,			                //00000001
            AcknowledgeEndOfInterrupt = 0x2,	//00000010
            MasterBuffer = 0x4,			        //00000100
            Buffered = 0x8,			            //00001000
            SpecialFullyNestedMode = 0x10	    //00010000
        };

        //Initialization Command 1 control bits
        enum class InitialisationCommand1ControlBit
        {
            IC4Expect = 1,			//1
            IC4No = 0,			    //0
            SingleYes = 2,			//10
            SingleNo = 0,			//00
            CallInterval4 = 4,	    //100
            CallInterval8 = 0,	    //000
            LevelTriggered = 8,     //1000
            EdgeTriggered = 0,	    //0000
            InitialiseYes = 0x10,	//10000
            InitialiseNo = 0		//00000
        };

        //Initialization Command 4 control bits
        enum class InitialisationCommand4ControlBit
        {
            uPM_86Mode = 1,			    //1
            uPM_MCSMode = 0,			//0
            AutoEndOfInterrupt = 2,			//10
            NoAutoEndOfInterrupt = 0,			//0
            BufferMaster	= 4,		//100
            BufferSlave = 0,			//0
            BufferedModeYes	= 8,			//1000
            BufferedModeNo = 0,			    //0
            Nested	= 0x10,		//10000
            NotNested = 0			//0
        };

        static PIC s_Instance;
    };
}
