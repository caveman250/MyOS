#ifndef _IDT_H
#define _IDT_H

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel
{
	class IDT
	{
	public:

		static void Initialise (uint16_t codeSel);

		struct Descriptor 
		{
			//bits 0-16 of interrupt routine (ir) address
			uint16_t		m_BaseLo;
			//code selector in gdt
			uint16_t		m_Sel;
			//reserved, shold be 0
			uint8_t			m_Reserved;
			uint8_t			m_Flags;
			//! bits 16-32 of ir address
			uint16_t		m_BaseHi;
		} __attribute__((packed));

		struct IDTR 
		{
			//! size of the interrupt descriptor table (idt)
			uint16_t		limit;

			//! base address of idt
			uint32_t		base;
		} __attribute((packed));

	private:

		//i86 defines 256 possible interrupt handlers
		#define I86_MAX_INTERRUPTS		256

		#define I86_IDT_DESC_BIT16		0x06	//00000110
		#define I86_IDT_DESC_BIT32		0x0E	//00001110
		#define I86_IDT_DESC_RING1		0x40	//01000000
		#define I86_IDT_DESC_RING2		0x20	//00100000
		#define I86_IDT_DESC_RING3		0x60	//01100000
		#define I86_IDT_DESC_PRESENT	0x80	//10000000

		static Descriptor* GetInterruptRoutine (uint32_t i);
		static void InstallInterruptRoutine (uint32_t i, uint16_t flags, uint16_t sel, uint32_t irq);
	};

	extern "C" void idt_install(uintptr_t);
}

#endif
