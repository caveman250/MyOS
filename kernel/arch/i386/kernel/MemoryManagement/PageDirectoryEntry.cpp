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
		m_Entry = (m_Entry & ~(uint32_t)Flags::Frame) | addr;
	}

	bool PageDirectoryEntry::IsPresent()
	{
		return m_Entry & (uint32_t)Flags::Present;
	}

	bool PageDirectoryEntry::IsWritable()
	{
		return m_Entry & (uint32_t)Flags::Writable;
	}

	physical_addr PageDirectoryEntry::GetFrameAddress()
	{
		return m_Entry & (uint32_t)Flags::Frame;
	}

	bool PageDirectoryEntry::IsUserMode()
	{
		return m_Entry & (uint32_t)Flags::User;
	}

	bool PageDirectoryEntry::Uses4MBPages()
	{
		return m_Entry & (uint32_t)Flags::FourMB;
	}
}