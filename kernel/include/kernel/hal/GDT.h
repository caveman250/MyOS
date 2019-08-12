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
		static int Initialise ();

		struct Descriptor 
		{
			//bits 0-15 of segment limit
			uint16_t m_Limit;
			//bits 0-23 of base address
			uint16_t m_BaseLo;
			uint8_t	 m_BaseMid;
			//descriptor access flags
			uint8_t m_Flags;
			uint8_t m_Grand;
			//bits 24-32 of base address
			uint8_t m_BaseHi;
		} __attribute((packed));

		struct GDTR 
		{
			//size of gdt
			uint16_t m_Limit;
			//base address of gdt
			uint32_t m_Base;
		} __attribute((packed));

		//maximum amount of descriptors allowed
		static constexpr uint8_t MAX_DESCRIPTORS = 3;

	private:
		enum class GDTDescriptorBit
		{
			ACCESS = 0x0001,		//00000001
			READWRITE = 0x0002,		//00000010
			EXPANSION = 0x0004,		//00000100
			EXEC_CODE = 0x0008,		//00001000
			CODEDATA = 0x0010,		//00010000
			DPL = 0x0060,			//01100000
			MEMORY = 0x0080			//10000000
		};

		enum class GDTDescriptorGrandBit
		{
			LIMITHI_MASK = 0x0f,	//00001111
			OS = 0x10,				//00010000
			THIRTY_TWO_BIT = 0x40,	//01000000
			FOUR_K = 0x80			//10000000
		};

		static void SetDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand);
		static Descriptor* GetDescriptor (int i);
	};

	extern "C" void gdt_install (uintptr_t, uint16_t);
}

#endif
