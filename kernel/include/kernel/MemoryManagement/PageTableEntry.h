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
			Present			=	1,			//0000000000000000000000000000001
			Writable		=	2,			//0000000000000000000000000000010
			User			=	4,			//0000000000000000000000000000100
			WriteThrough	=	8,			//0000000000000000000000000001000
			CacheDisabled	=	0x10,		//0000000000000000000000000010000
			Accessed		=	0x20,		//0000000000000000000000000100000
			Dirty			=	0x40,		//0000000000000000000000001000000
			PAT				=	0x80,		//0000000000000000000000010000000
			CpuGlobal		=	0x100,		//0000000000000000000000100000000
			LevelFourGlobal	=	0x200,		//0000000000000000000001000000000
			Frame			=	0x7FFFF000 	//1111111111111111111000000000000
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