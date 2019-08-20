#include <kernel/hal/IDT.h>
#include <string.h>
#include <stdio.h>
#include <kernel/hal/HAL.h>
#include <kernel/Terminal.h>
#include <kernel/Exception.h>

namespace kernel
{
	IDT IDT::s_Instance;

	extern "C" void default_irq_handler() 
	{
		Exception::KernelPanic("Unhandled Exception!");
		while(true);
	}

	void IDT::Initialise(uint16_t codeSel) 
	{
		m_Idtr.limit = sizeof (Descriptor) * s_MaxInterrupts -1;
		m_Idtr.base	= (uint32_t)&m_Idt[0];

		//null out the idt
		memset ((void*)&m_Idt[0], 0, sizeof (Descriptor) * s_MaxInterrupts-1);

		//register default handler for all interrupts
		for (int i=0; i<s_MaxInterrupts; i++)
		{
			InstallInterruptRoutine (i, 
				(uint16_t)IDTDescriptorBit::Present | (uint16_t)IDTDescriptorBit::Bit32,
				codeSel, 
				(uint32_t)default_irq_handler);
		}

		idt_install ((intptr_t)&m_Idtr);
	}

	IDT::Descriptor* IDT::GetInterruptDescriptor(uint32_t i) 
	{
		if (i > s_MaxInterrupts)
		{
			return 0;
		}

		return &m_Idt[i];
	}

	void IDT::InstallInterruptRoutine(uint32_t i, uint16_t flags, uint16_t sel, uint32_t irq) 
	{
		if (i > s_MaxInterrupts)
			return;

		m_Idt[i].m_BaseLo = irq & 0xffff;
		m_Idt[i].m_BaseHi = (irq & 0xffff0000) >> 16;
		m_Idt[i].m_Reserved = 0;
		m_Idt[i].m_Flags = uint8_t(flags);
		m_Idt[i].m_Sel = sel;
	}
}
