#include <kernel/hal/HAL.h>
#include <kernel/hal/CPU.h>
#include <kernel/hal/IDT.h>
#include <kernel/hal/PIC.h>
#include <kernel/hal/PIT.h>

namespace kernel::hal
{
	HAL HAL::s_Instance;

	void HAL::Initialise() 
	{
		CPU::Get().Initialise();
		PIC::Get().Initialise(0x20, 0x28);
		PIT::Get().Initialise();
		PIT::Get().StartCounter(100, (uint8_t)PIT::Counter::Counter0, (uint8_t)PIT::CounterMode::SqureWaveGen);

		EnableInterrupts();
	}

	void HAL::Shutdown()
	{
		CPU::Get().Shutdown();
	}

	void HAL::InterruptFinished(unsigned int intno) 
	{
		//ensure its a valid hardware irq
		if (intno > 16)
			return;

		//do we need to notify the second pic?
		if (intno >= 8)
		{
			PIC::Get().SendCommand((uint8_t)PIC::CommandWord2Mask::EndOfInterrupt, 1);
		}

		//always send end-of-interrupt to primary pic
		PIC::Get().SendCommand((uint8_t)PIC::CommandWord2Mask::EndOfInterrupt, 0);
	}

	void HAL::Sound(unsigned frequency) 
	{
		//sets frequency for speaker. frequency of 0 disables speaker
		OutB(0x61, 3 | (unsigned char)(frequency << 2) );
	}

	unsigned char HAL::InB(unsigned short portid) 
	{
		uint8_t ret;
		asm volatile ( "inb %0, %1"
						: "=a"(ret)
						: "Nd"(portid) );
		return ret;
	}

	void HAL::OutB(unsigned short portid, unsigned char value) 
	{
		asm volatile ( "outb %0, %1" 
		: : "Nd"(portid), "a"(value) );
	}

	void HAL::EnableInterrupts() 
	{
		asm volatile ("sti");
	}

	void HAL::DisableInterrupts() 
	{
		asm volatile ("cli");
	}

	void HAL::SetInterruptRoutine(int intno, uint32_t vect) 
	{
		//install interrupt handler. This overwrites default interrupt handler
		IDT::Get().InstallInterruptRoutine(intno, (uint16_t)IDT::IDTDescriptorBit::Present | (uint16_t)IDT::IDTDescriptorBit::Bit32, 0x8, vect);
	}

	void (*HAL::GetInterruptRoutine(int intno))() 
	{
		IDT::Descriptor* desc = IDT::Get().GetInterruptDescriptor(intno);
		if (!desc)
		{
			return 0;
		}

		//get address of interrupt handler
		uint32_t addr = desc->m_BaseLo | (desc->m_BaseHi << 16);

		return (void (*)(void))addr;
	}
	
	const char* HAL::GetCpuVendor() 
	{
		return CPU::Get().GetVendor();
	}

	int HAL::GetTickCount() 
	{
		return PIT::Get().GetTickCount();
	}
}