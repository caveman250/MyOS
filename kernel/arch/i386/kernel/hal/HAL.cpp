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
		PIT::StartCounter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

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
			PIC::SendCommand(I86_PIC_OCW2_MASK_EOI, 1);
		}

		//always send end-of-interrupt to primary pic
		PIC::SendCommand(I86_PIC_OCW2_MASK_EOI, 0);
	}

	void HAL::Sound(unsigned frequency) 
	{
		//sets frequency for speaker. frequency of 0 disables speaker
		OutB(0x61, 3 | (unsigned char)(frequency << 2) );
	}

	unsigned char HAL::InB(unsigned short portid) 
	{
		uint8_t ret;
		asm volatile ( "inb %1, %0"
						: "=a"(ret)
						: "Nd"(portid) );
		return ret;
	}

	void HAL::OutB(unsigned short portid, unsigned char value) 
	{
		asm volatile ( "outb %0, %1" : : "a"(value), "Nd"(portid) );
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
		IDT::InstallInterruptRoutine(intno, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, 0x8, vect);
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