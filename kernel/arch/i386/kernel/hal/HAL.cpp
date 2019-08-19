#include <kernel/hal/HAL.h>
#include <kernel/hal/CPU.h>
#include <kernel/hal/IDT.h>
#include <kernel/hal/PIC.h>
#include <kernel/hal/PIT.h>

namespace kernel
{
	void HAL::Initialise() 
	{
		CPU::Initialise();
		PIC::Initialise(0x20, 0x28);
		PIT::Initialise();
		PIT::StartCounter(100, (uint8_t)PIT::Counter::COUNTER_0, (uint8_t)PIT::CounterMode::SQUAREWAVEGEN);

		EnableInterrupts();
	}

	void HAL::Shutdown()
	{
		CPU::Shutdown();
	}

	void HAL::InterruptFinished(unsigned int intno) 
	{
		//ensure its a valid hardware irq
		if (intno > 16)
			return;

		//do we need to notify the second pic?
		if (intno >= 8)
		{
			PIC::SendCommand((uint8_t)PIC::CommandWord2Mask::EOI, 1);
		}

		//always send end-of-interrupt to primary pic
		PIC::SendCommand((uint8_t)PIC::CommandWord2Mask::EOI, 0);
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
		asm volatile ( "outb %0, %1" : : "Nd"(portid), "a"(value) );
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
		IDT::InstallInterruptRoutine(intno, (uint16_t)IDT::IDTDescriptorBit::PRESENT | (uint16_t)IDT::IDTDescriptorBit::BIT32, 0x8, vect);
	}

	void (*HAL::GetInterruptRoutine(int intno))() 
	{
		IDT::Descriptor* desc = IDT::GetInterruptDescriptor(intno);
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
		return CPU::GetVendor();
	}

	int HAL::GetTickCount() 
	{
		return PIT::GetTickCount();
	}
}