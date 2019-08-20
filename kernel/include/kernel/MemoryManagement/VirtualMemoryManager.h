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

		static VirtualMemoryManager& Get() { return s_Instance; }

		VirtualMemoryManager();

		void Initialise();
		bool AllocatePage(PageTableEntry& PageTableEntry);
		void FreePage(PageTableEntry& PageTableEntry);

		physical_addr GetPhysicalAddress(virtual_addr addr);

	private:
		bool SwitchPageDirectory(PageDirectory* directory);
		PageDirectory* GetPageDirectory();

		static inline int GetPageDirectoryIndex(virtual_addr addr) { return (addr >> 22) & 0x3ff; }
		static inline int GetPageTableIndex(virtual_addr addr) { return (addr >> 12) & 0x3ff; }
		static inline void* GetPagePhysicalAddress(pd_entry* entry) { return (void*)(*entry & ~0xfff); }

		PageDirectory* m_CurrentDirectory;
		physical_addr m_CurrentPDBR;

		static VirtualMemoryManager s_Instance;
	};
}
