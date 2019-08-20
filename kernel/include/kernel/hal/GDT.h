#pragma once

#if !defined(__i386__)
#error "[cpu.h] platform not implimented. "
#endif

#include <stdint.h>

namespace kernel::hal
{
	class GDT
	{
	public:
		static GDT& Get() { return s_Instance; } 
		
		int Initialise();

	private:
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

		enum class GDTDescriptorBit
		{
			Access = 0x0001,			//00000001
			ReadWrite = 0x0002,			//00000010
			Expansion = 0x0004,			//00000100
			ExecuteCode = 0x0008,		//00001000
			CodeData = 0x0010,			//00010000
			PrivilegeLevel = 0x0060,	//01100000
			Memory = 0x0080				//10000000
		};

		enum class GDTDescriptorGrandBit
		{
			LimithiMask = 0x0f,			//00001111
			OperatingSystem = 0x10,		//00010000
			ThirtyTwoBit = 0x40,		//01000000
			FourK = 0x80				//10000000
		};

		void SetDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand);
		Descriptor* GetDescriptor (int i);

		//maximum amount of descriptors allowed
		static constexpr uint8_t s_MaxDescriptors = 3;
		
		Descriptor m_Gdt[s_MaxDescriptors] __attribute__((aligned(8)));
		GDTR m_Gdtr __attribute__((aligned(8)));

		static GDT s_Instance;
	};

	extern "C" void gdt_install (uintptr_t, uint16_t);
}
