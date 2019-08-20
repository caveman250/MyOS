#pragma once

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel
{
	class IDT
	{
	public:
		struct Descriptor 
		{
			//bits 0-16 of interrupt routine (ir) address
			uint16_t		m_BaseLo;
			//code selector in gdt
			uint16_t		m_Sel;
			//reserved, shold be 0
			uint8_t			m_Reserved;
			uint8_t			m_Flags;
			//bits 16-32 of ir address
			uint16_t		m_BaseHi;
		} __attribute__((packed));

		enum class IDTDescriptorBit
		{
			Bit16 = 0x06,	//00000110
			Bit32 = 0x0E,	//00001110
			Ring1 = 0x40,	//01000000
			Ring2 = 0x20,	//00100000
			Ring3 = 0x60,	//01100000
			Present	= 0x80	//10000000
		};

		static IDT& Get() { return s_Instance; }

		void Initialise(uint16_t codeSel);

		Descriptor* GetInterruptDescriptor(uint32_t i);
		void InstallInterruptRoutine(uint32_t i, uint16_t flags, uint16_t sel, uint32_t irq);

	private:
		//i86 defines 256 possible interrupt handlers
		static constexpr int s_MaxInterrupts = 256;

		struct IDTR 
		{
			//size of the interrupt descriptor table (idt)
			uint16_t		limit;

			//base address of idt
			uint32_t		base;
		} __attribute((packed));

		Descriptor m_Idt[s_MaxInterrupts] __attribute__((aligned(8)));
		IDTR m_Idtr __attribute__((aligned(8)));

		static IDT s_Instance;
	};

	extern "C" void idt_install(uintptr_t);
}
