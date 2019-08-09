#ifndef _GDT_H_INCLUDED
# define _GDT_H_INCLUDED

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel
{
	class GDT
	{
	public:
		
		//! initializes gdt
		static int Initialise ();

		struct Descriptor 
		{
			//! bits 0-15 of segment limit
			uint16_t m_Limit;
			//! bits 0-23 of base address
			uint16_t m_BaseLo;
			uint8_t	 m_BaseMid;
			//! descriptor access flags
			uint8_t m_Flags;
			uint8_t m_Grand;
			//! bits 24-32 of base address
			uint8_t m_BaseHi;
		} __attribute((packed));

		struct GDTR 
		{
			//! size of gdt
			uint16_t m_Limit;
			//! base address of gdt
			uint32_t m_Base;
		} __attribute((packed));

	private:
		//! maximum amount of descriptors allowed
		#define MAX_DESCRIPTORS					3

		#define I86_GDT_DESC_ACCESS				0x0001			//00000001
		#define I86_GDT_DESC_READWRITE			0x0002			//00000010
		#define I86_GDT_DESC_EXPANSION			0x0004			//00000100
		#define I86_GDT_DESC_EXEC_CODE			0x0008			//00001000
		#define I86_GDT_DESC_CODEDATA			0x0010			//00010000
		#define I86_GDT_DESC_DPL				0x0060			//01100000
		#define I86_GDT_DESC_MEMORY				0x0080			//10000000
		#define I86_GDT_GRAND_LIMITHI_MASK		0x0f			//00001111
		#define I86_GDT_GRAND_OS				0x10			//00010000
		#define I86_GDT_GRAND_32BIT				0x40			//01000000
		#define I86_GDT_GRAND_4K				0x80			//10000000

		static void SetDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand);
		static Descriptor* GetDescriptor (int i);
	};

	extern "C" void gdt_install (uintptr_t, uint16_t);
}

#endif
