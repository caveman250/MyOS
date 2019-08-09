#include <kernel/hal/GDT.h>
#include <string.h>
#include <stdio.h>

namespace kernel
{
	static GDT::Descriptor s_Gdt [MAX_DESCRIPTORS] __attribute__((aligned(8)));
	static GDT::GDTR s_Gdtr __attribute__((aligned(8)));

	int GDT::Initialise () 
	{
		//set up GDTR
		s_Gdtr.m_Limit = (sizeof (struct Descriptor) * MAX_DESCRIPTORS)-1;
		s_Gdtr.m_Base = (uint32_t)&s_Gdt[0];

		//null descriptor
		SetDescriptor(0, 0, 0, 0, 0);

		//default code descriptor
		SetDescriptor (1,
			0,
			0xffffffff,
			I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
			I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

		//default data descriptor
		SetDescriptor (2,
			0,
			0xffffffff,
			I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
			I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

		gdt_install ((uintptr_t)&s_Gdt, sizeof(s_Gdt) - 1);

		return 0;
	}

	void GDT::SetDescriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
	{
		if (i > MAX_DESCRIPTORS)
		{
			return;
		}

		//null out the descriptor
		memset ((void*)&s_Gdt[i], 0, sizeof (Descriptor));

		s_Gdt[i].m_BaseLo	= uint16_t(base & 0xffff);
		s_Gdt[i].m_BaseMid	= uint8_t((base >> 16) & 0xff);
		s_Gdt[i].m_BaseHi	= uint8_t((base >> 24) & 0xff);
		s_Gdt[i].m_Limit	= uint16_t(limit & 0xffff);
		s_Gdt[i].m_Flags = access;
		s_Gdt[i].m_Grand = uint8_t((limit >> 16) & 0x0f);
		s_Gdt[i].m_Grand |= grand & 0xf0;
	}

	GDT::Descriptor* GDT::GetDescriptor (int i)
	{
		if (i > MAX_DESCRIPTORS)
		{
			return 0;
		}

		return &s_Gdt[i];
	}
}
