#include <kernel/hal/IDT.h>
#include <string.h>
#include <stdio.h>
#include <kernel/hal/HAL.h>
#include <kernel/Terminal.h>
#include <kernel/Vga.h>
#include <kernel/Exception.h>

namespace kernel
{
	static IDT::Descriptor s_Idt [I86_MAX_INTERRUPTS] __attribute__((aligned(8)));
	static IDT::IDTR s_Idtr __attribute__((aligned(8)));

	extern "C" void default_irq_handler() 
	{
		Exception::KernelPanic("Unhandled Exception!");
		while(true);
	}

	void IDT::Initialise(uint16_t codeSel) 
	{
		s_Idtr.limit = sizeof (Descriptor) * I86_MAX_INTERRUPTS -1;
		s_Idtr.base	= (uint32_t)&s_Idt[0];

		//! null out the idt
		memset ((void*)&s_Idt[0], 0, sizeof (Descriptor) * I86_MAX_INTERRUPTS-1);

		//register default handler for all interrupts
		for (int i=0; i<I86_MAX_INTERRUPTS; i++)
		{
			InstallInterruptRoutine (i, 
				I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
				codeSel, 
				(uint32_t)default_irq_handler);
		}

		idt_install ((intptr_t)&s_Idtr);
	}

	IDT::Descriptor* IDT::GetInterruptDescriptor(uint32_t i) 
	{
		if (i > I86_MAX_INTERRUPTS)
		{
			return 0;
		}

		return &s_Idt[i];
	}

	void IDT::InstallInterruptRoutine(uint32_t i, uint16_t flags, uint16_t sel, uint32_t irq) 
	{
		if (i>I86_MAX_INTERRUPTS)
			return;

		s_Idt[i].m_BaseLo = irq & 0xffff;
		s_Idt[i].m_BaseHi = (irq & 0xffff0000) >> 16;
		s_Idt[i].m_Reserved = 0;
		s_Idt[i].m_Flags = uint8_t(flags);
		s_Idt[i].m_Sel = sel;
	}
}
