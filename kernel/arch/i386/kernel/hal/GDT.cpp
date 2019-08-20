#include <kernel/hal/GDT.h>
#include <string.h>
#include <stdio.h>

namespace kernel::hal
{
	GDT GDT::s_Instance;

	int GDT::Initialise()
	{
		//set up GDTR
		m_Gdtr.m_Limit = (sizeof(struct Descriptor) * s_MaxDescriptors) - 1;
		m_Gdtr.m_Base = (uint32_t)&m_Gdt[0];

		//null descriptor
		SetDescriptor(0, 0, 0, 0, 0);

		//default code descriptor
		SetDescriptor(1,
			0,
			0xffffffff,
			(uint8_t)GDTDescriptorBit::ReadWrite | (uint8_t)GDTDescriptorBit::ExecuteCode | (uint8_t)GDTDescriptorBit::CodeData | (uint8_t)GDTDescriptorBit::Memory,
			(uint8_t)GDTDescriptorGrandBit::FourK | (uint8_t)GDTDescriptorGrandBit::ThirtyTwoBit | (uint8_t)GDTDescriptorGrandBit::LimithiMask);

		//default data descriptor
		SetDescriptor(2,
			0,
			0xffffffff,
			(uint8_t)GDTDescriptorBit::ReadWrite | (uint8_t)GDTDescriptorBit::CodeData | (uint8_t)GDTDescriptorBit::Memory,
			(uint8_t)GDTDescriptorGrandBit::FourK | (uint8_t)GDTDescriptorGrandBit::ThirtyTwoBit | (uint8_t)GDTDescriptorGrandBit::LimithiMask);

		gdt_install((uintptr_t)&m_Gdt, sizeof(m_Gdt) - 1);

		return 0;
	}

	void GDT::SetDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
	{
		if (i > s_MaxDescriptors)
		{
			return;
		}

		//null out the descriptor
		memset ((void*)&m_Gdt[i], 0, sizeof (Descriptor));

		m_Gdt[i].m_BaseLo	= uint16_t(base & 0xffff);
		m_Gdt[i].m_BaseMid	= uint8_t((base >> 16) & 0xff);
		m_Gdt[i].m_BaseHi	= uint8_t((base >> 24) & 0xff);
		m_Gdt[i].m_Limit	= uint16_t(limit & 0xffff);
		m_Gdt[i].m_Flags = access;
		m_Gdt[i].m_Grand = uint8_t((limit >> 16) & 0x0f);
		m_Gdt[i].m_Grand |= grand & 0xf0;
	}

	GDT::Descriptor* GDT::GetDescriptor (int i)
	{
		if (i > s_MaxDescriptors)
		{
			return 0;
		}

		return &m_Gdt[i];
	}
}
