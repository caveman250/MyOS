#pragma once

#include <stdint.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>

namespace kernel::memory
{
	typedef uint32_t pt_entry;

	class PageTableEntry
	{
	public:
		enum class Flags 
		{
			PRESENT			=	1,			//0000000000000000000000000000001
			WRITABLE		=	2,			//0000000000000000000000000000010
			USER			=	4,			//0000000000000000000000000000100
			WRITETHOUGH		=	8,			//0000000000000000000000000001000
			NOT_CACHEABLE	=	0x10,		//0000000000000000000000000010000
			ACCESSED		=	0x20,		//0000000000000000000000000100000
			DIRTY			=	0x40,		//0000000000000000000000001000000
			PAT				=	0x80,		//0000000000000000000000010000000
			CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
			LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
			FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
		};

		PageTableEntry();

		void AddAttribute(uint32_t attrib);
		void DeleteAttribute(uint32_t attrib);
		void SetFrame(physical_addr addr);
		bool IsPresent();
		bool IsWritable();
		physical_addr GetFrameAddress();

	private:
		pt_entry m_Entry;
	};
}