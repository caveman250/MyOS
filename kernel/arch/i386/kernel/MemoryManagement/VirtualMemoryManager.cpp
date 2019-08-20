#include <string.h>
#include <stdio.h>
#include <kernel/MemoryManagement/VirtualMemoryManager.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>

namespace kernel::memory
{
   VirtualMemoryManager::PageDirectory* VirtualMemoryManager::s_CurrentDirectory = 0;
   physical_addr VirtualMemoryManager::s_CurrentPDBR = 0;

   bool VirtualMemoryManager::SwitchPageDirectory(PageDirectory* dir) 
   {
      if (!dir)
      {
         return false;
      }

      s_CurrentPDBR = (physical_addr) &dir->m_Entries;
      s_CurrentDirectory = dir;

      PhysicalMemoryManager::LoadPDBR(s_CurrentPDBR);
      return true;
   }

   VirtualMemoryManager::PageDirectory* VirtualMemoryManager::GetPageDirectory()
   {
      return s_CurrentDirectory;
   }

   bool VirtualMemoryManager::AllocatePage(PageTableEntry& pte)
   {
      //allocate a free physical frame
      void* p = PhysicalMemoryManager::AllocateBlock();
      if (!p)
      {
         return false;
      }

      //map it to the page
      pte.SetFrame((physical_addr)p);
      pte.AddAttribute((uint32_t)PageTableEntry::Flags::PRESENT);

      return true;
   }

   void VirtualMemoryManager::FreePage(PageTableEntry& pte)
   {
      if (void* p = (void*)pte.GetFrameAddress())
      {
         PhysicalMemoryManager::FreeBlock(p);
      }

      pte.DeleteAttribute((uint32_t)PageTableEntry::Flags::PRESENT);
   }

   void VirtualMemoryManager::Initialise()
   {
      PageTable* table = (PageTable*)PhysicalMemoryManager::AllocateBlock();
      PageTable* table2 = (PageTable*)PhysicalMemoryManager::AllocateBlock();
      
      if (!table || !table2)
      {
         return;
      }

      //clear page tables
      memset (table, 0, sizeof(PageTable));
      memset (table2, 0, sizeof(PageTable));

      //1st 4mb are idenitity mapped
      for (int i=0, frame=0x0, virt=0x00000000; i < 1024; i++, frame+=4096, virt+=4096) 
      {
         PageTableEntry pte;
         pte.AddAttribute((uint32_t)PageTableEntry::Flags::PRESENT);
         pte.SetFrame(frame);

         table2->m_Entries[GetPageTableIndex(virt)] = pte;
      }

      //map 0mb to 3gb (kernel)
      for (int i=0, frame=0x0, virt=0xc0000000; i < 1024; i++, frame+=4096, virt+=4096) 
      {
         PageTableEntry pte;
         pte.AddAttribute((uint32_t)PageTableEntry::Flags::PRESENT);
         pte.SetFrame(frame);

         table->m_Entries[GetPageTableIndex(virt)] = pte;
      }

      //create default directory table
      PageDirectory* dir = (PageDirectory*)PhysicalMemoryManager::AllocateBlocks(3);
      if (!dir)
      {
         return;
      }

      memset(dir, 0, sizeof(PageDirectory));

      PageDirectoryEntry& pde = dir->m_Entries[GetPageDirectoryIndex(0xc0000000)];
      pde.AddAttribute((uint32_t)PageDirectoryEntry::Flags::PRESENT);
      pde.AddAttribute((uint32_t)PageDirectoryEntry::Flags::WRITABLE);
      pde.SetFrame((physical_addr)table);

      PageDirectoryEntry& pde2 = dir->m_Entries[GetPageDirectoryIndex(0x00000000)];
      pde2.AddAttribute((uint32_t)PageDirectoryEntry::Flags::PRESENT);
      pde2.AddAttribute((uint32_t)PageDirectoryEntry::Flags::WRITABLE);
      pde2.SetFrame((physical_addr)table2);

      SwitchPageDirectory(dir);
      PhysicalMemoryManager::SetPagingEnabled(true);
   }

   physical_addr VirtualMemoryManager::GetPhysicalAddress(virtual_addr addr)
   {
      int dirIndex = GetPageDirectoryIndex(addr);
      if(PageDirectoryEntry* pde = &(s_CurrentDirectory->m_Entries[dirIndex]))
      {
         physical_addr pdeFrameAddr = pde->GetFrameAddress();
         if(PageTable* pageTable = (PageTable*)pdeFrameAddr)
         {
            int tableIndex = GetPageTableIndex(addr);
            if(PageTableEntry* pte = &(pageTable->m_Entries[tableIndex]))
            {
               return pte->GetFrameAddress();
            }
         }
      }

      return 0;
   }
}
