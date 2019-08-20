
#if !defined(__i386__)
#error "[pic.cpp for i86] requires i86 architecture."
#endif

#include <kernel/hal/HAL.h>
#include <kernel/hal/PIC.h>

namespace kernel
{
	PIC PIC::s_Instance;

	void PIC::Initialise(uint8_t base0, uint8_t base1) 
	{
		uint8_t icw	= 0;

		kernel::hal::HAL::Get().DisableInterrupts();

		//Begin initialization of PIC
		icw = (icw & ~(uint8_t)InitialisationWord1Mask::Initialise) | (uint8_t)InitialisationCommand1ControlBit::InitialiseYes;
		icw = (icw & ~(uint8_t)InitialisationWord1Mask::IC4) | (uint8_t)InitialisationCommand1ControlBit::IC4Expect;

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
		icw = (icw & ~(uint8_t)InitialisationWord4Mask::uPM) | (uint8_t)InitialisationCommand4ControlBit::uPM_86Mode;

		SendData(icw, 0);
		SendData(icw, 1);
	}

	void PIC::SendCommand(uint8_t cmd, uint8_t picNum) 
	{
		if (picNum > 1)
		{
			return;
		}

		uint8_t	reg = (picNum==1) ? (uint8_t)PIC2PortAddress::Command : (uint8_t)PIC1PortAddress::Command;
		kernel::hal::HAL::Get().OutB(reg, cmd);
	}

	void PIC::SendData(uint8_t data, uint8_t picNum)
	{
		if (picNum > 1)
		{
			return;
		}

		uint8_t	reg = (picNum == 1) ? (uint8_t)PIC2PortAddress::Data : (uint8_t)PIC1PortAddress::Data;
		kernel::hal::HAL::Get().OutB(reg, data);
	}

	uint8_t PIC::ReadData(uint8_t picNum)
	{
		if (picNum > 1)
		{
			return 0;
		}

		uint8_t	reg = (picNum == 1) ? (uint8_t)PIC2PortAddress::Data : (uint8_t)PIC1PortAddress::Data;
		return kernel::hal::HAL::Get().InB(reg);
	}
}