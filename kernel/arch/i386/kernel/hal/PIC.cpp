
#if !defined(__i386__)
#error "[pic.cpp for i86] requires i86 architecture."
#endif

#include <kernel/hal/HAL.h>
#include <kernel/hal/PIC.h>

namespace kernel
{
	//PIC 1 register port addresses
    enum class PIC1PortAddress
    {
        COMMAND = 0x20,
        STATUS	= 0x20,
        DATA = 0x21,
        IMR = 0x21
    };

	//PIC 2 register port addresses
    enum class PIC2PortAddress
    {
        COMMAND = 0xA0,
        STATUS	= 0xA0,
        DATA = 0xA1,
        IMR = 0xA1
    };

	//Initialisation Control Word 1 bit masks
    enum class InitialisationWord1Mask
    {
	    IC4 = 0x1,			        //00000001
	    SNGL = 0x2,			        //00000010
	    ADI = 0x4,			        //00000100
	    LTIM = 0x8,			        //00001000
	    INIT = 0x10		            //00010000
    };

	//Initialisation Control Words 2 and 3 do not require bit masks

	//Initialisation Control Word 4 bit masks
    enum class InitialisationWord4Mask
    {
        UPM	= 0x1,			        //00000001
        AEOI = 0x2,			        //00000010
        MS = 0x4,			        //00000100
        BUF = 0x8,			        //00001000
        SFNM = 0x10	                //00010000
    };

	//Initialization Command 1 control bits
    enum class InitialisationCommand1ControlBit
    {
        IC4_EXPECT = 1,			    //1
        IC4_NO = 0,			        //0
        SNGL_YES = 2,			    //10
        SNGL_NO = 0,			    //00
        ADI_CALLINTERVAL4 = 4,	    //100
        ADI_CALLINTERVAL8 = 0,	    //000
        LTIM_LEVELTRIGGERED = 8,    //1000
        LTIM_EDGETRIGGERED = 0,	    //0000
        INIT_YES = 0x10,		    //10000
        INIT_NO = 0			        //00000
    };

	//Initialization Command 4 control bits
    enum class InitialisationCommand4ControlBit
    {
        UPM_86MODE = 1,			    //1
        UPM_MCSMODE = 0,			//0
        AEOI_AUTOEOI = 2,			//10
        AEOI_NOAUTOEOI = 0,			//0
        MS_BUFFERMASTER	= 4,		//100
        MS_BUFFERSLAVE = 0,			//0
        BUF_MODEYES	= 8,			//1000
        BUF_MODENO = 0,			    //0
        SFNM_NESTEDMODE	= 0x10,		//10000
        SFNM_NOTNESTED = 0			//0
    };

	void PIC::SendCommand(uint8_t cmd, uint8_t picNum) 
	{
		if (picNum > 1)
		{
			return;
		}

		uint8_t	reg = (picNum==1) ? (uint8_t)PIC2PortAddress::COMMAND : (uint8_t)PIC1PortAddress::COMMAND;
		kernel::hal::HAL::OutB(reg, cmd);
	}

	void PIC::SendData(uint8_t data, uint8_t picNum)
	{
		if (picNum > 1)
		{
			return;
		}

		uint8_t	reg = (picNum == 1) ? (uint8_t)PIC2PortAddress::DATA : (uint8_t)PIC1PortAddress::DATA;
		kernel::hal::HAL::OutB(reg, data);
	}

	uint8_t PIC::ReadData(uint8_t picNum)
	{
		if (picNum > 1)
		{
			return 0;
		}

		uint8_t	reg = (picNum == 1) ? (uint8_t)PIC2PortAddress::DATA : (uint8_t)PIC1PortAddress::DATA;
		return kernel::hal::HAL::InB(reg);
	}

	void PIC::Initialise(uint8_t base0, uint8_t base1) 
	{
		uint8_t icw	= 0;

		kernel::hal::HAL::DisableInterrupts();

		//Begin initialization of PIC
		icw = (icw & ~(uint8_t)InitialisationWord1Mask::INIT) | (uint8_t)InitialisationCommand1ControlBit::INIT_YES;
		icw = (icw & ~(uint8_t)InitialisationWord1Mask::IC4) | (uint8_t)InitialisationCommand1ControlBit::IC4_EXPECT;

		SendCommand(icw, 0);
		SendCommand(icw, 1);

		//Send initialization control word 2. This is the base addresses of the irq's
		SendData(base0, 0);
		SendData(base1, 1);

		//Send initialization control word 3. This is the connection between master and slave.
		//ICW3 for master PIC is the IR that connects to secondary pic in binary format
		//ICW3 for secondary PIC is the IR that connects to master pic in decimal format

		SendData(0x04, 0);
		SendData(0x02, 1);

		//Send Initialization control word 4. Enables i86 mode
		icw = (icw & ~(uint8_t)InitialisationWord4Mask::UPM) | (uint8_t)InitialisationCommand4ControlBit::UPM_86MODE;

		SendData(icw, 0);
		SendData(icw, 1);
	}
}