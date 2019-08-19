#include <kernel/MemoryManagement/PageDirectoryEntry.h>

namespace kernel::memory
{
	PageDirectoryEntry::PageDirectoryEntry(pd_entry entry)
		: m_Entry(entry)
	{
	}

	void PageDirectoryEntry::AddAttribute(uint32_t attrib) 
	{
		m_Entry |= attrib;
	}

	void PageDirectoryEntry::DeleteAttribute(uint32_t attrib)
	{
		m_Entry &= ~attrib;
	}

	void PageDirectoryEntry::SetFrame(physical_addr addr)
	{
		m_Entry = (m_Entry & ~Flags::FRAME) | addr;
	}

	bool PageDirectoryEntry::IsPresent()
	{
		return m_Entry & Flags::PRESENT;
	}

	bool PageDirectoryEntry::IsWritable()
	{
		return m_Entry & Flags::WRITABLE;
	}

	physical_addr PageDirectoryEntry::GetFrameAddress()
	{
		return m_Entry & Flags::FRAME;
	}

	bool PageDirectoryEntry::IsUserMode()
	{
		return m_Entry & Flags::USER;
	}

	bool PageDirectoryEntry::Uses4MBPages()
	{
		return m_Entry & Flags::FOURMB;
	}
}