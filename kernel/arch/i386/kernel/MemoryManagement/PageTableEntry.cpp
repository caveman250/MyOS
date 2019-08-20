#include <kernel/MemoryManagement/PageTableEntry.h>

namespace kernel::memory
{
	PageTableEntry::PageTableEntry()
		: m_Entry(0)
	{
	}

	void PageTableEntry::AddAttribute(uint32_t attrib)
	{
		m_Entry |= attrib;
	}

	void PageTableEntry::DeleteAttribute(uint32_t attrib)
	{
		m_Entry &= ~attrib;
	}

	void PageTableEntry::SetFrame(physical_addr addr)
	{
		m_Entry = (m_Entry & ~(uint32_t)Flags::Frame) | addr;
	}

	bool PageTableEntry::IsPresent()
	{
		return m_Entry & (uint32_t)Flags::Present;
	}

	bool PageTableEntry::IsWritable() 
	{
		return m_Entry & (uint32_t)Flags::Writable;
	}

	physical_addr PageTableEntry::GetFrameAddress()
	{
		return m_Entry & (uint32_t)Flags::Frame;
	}
}
