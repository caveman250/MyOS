#pragma once

#include <stdint.h>
#include <kernel/MemoryManagement/PageTableEntry.h>
#include <kernel/MemoryManagement/PageDirectoryEntry.h>

namespace kernel::memory
{
	typedef uint32_t virtual_addr;

	class VirtualMemoryManager
	{
	public:
		struct PageTable 
		{
			static constexpr int s_PagesPerTable = 1024;

			PageTableEntry m_Entries[s_PagesPerTable];
		};

		struct PageDirectory 
		{
			static constexpr int s_PagesPerDir	= 1024;

			PageDirectoryEntry m_Entries[s_PagesPerDir];
		};

		static void Initialise();
		static bool AllocatePage(PageTableEntry& PageTableEntry);
		static void FreePage(PageTableEntry& PageTableEntry);

	private:
		static bool SwitchPageDirectory(PageDirectory* directory);
		static PageDirectory* GetPageDirectory();

		static inline int GetPageDirectoryIndex(virtual_addr addr) { return (addr >> 22) & 0x3ff; }
		static inline int GetPageTableIndex(virtual_addr addr) { return (addr >> 12) & 0x3ff; }
		static inline void* GetPagePhysicalAddress(pd_entry* entry) { return (void*)(*entry & ~0xfff); }

		static PageDirectory* s_CurrentDirectory;
		static physical_addr s_CurrentPDBR;
	};
}
