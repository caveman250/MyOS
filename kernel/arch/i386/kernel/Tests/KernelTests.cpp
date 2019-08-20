#include <kernel/Tests/KernelTests.h>
#include <stdio.h>
#include <stdint.h>

#include <vendor/multiboot.h>

#include <kernel/kernel.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
#include <kernel/hal/HAL.h>
#include <kernel/Terminal.h>

namespace kernel
{
    void KernelTests::MemoryMap()
    {
        Terminal::ClearScreen();
        printf("Physical Memory Map Test\n\n");

        const char* MemoryTypeStrings[] = 
        {
            "Available",
            "Reserved",
            "ACPI Reclaim",
            "ACPI NVS Memory",
            "Bad Ram"
        };

        multiboot_info_t* mbt = (multiboot_info_t*)0x10000;

        printf("mbt ptr: 0x%x\n", mbt);
		printf("Memory Lower: 0x%x\n", mbt->mem_lower);
		printf("Memory Upper: 0x%x\n", mbt->mem_upper);

        int32_t memSize = 1024 + mbt->mem_lower + mbt->mem_upper;
		printf("Memory Size: 0x%x\n", memSize);
        printf("Memory Map Length: 0x%x\n", mbt->mmap_length);

		printf("\nPhysical Memory Map:\n");
        int mapIndex = 0;
        for (multiboot_memory_map_t* memMap = (multiboot_memory_map_t*)mbt->mmap_addr; (uint32_t)memMap < (uint32_t)(mbt->mmap_addr + mbt->mmap_length); memMap++) 
		{
			printf("    region %i: start: 0x%x%x length bytes: 0x%x%x type: %s\n", 
				mapIndex, 
				memMap->addrHi, 
				memMap->addrLow,
				memMap->lenHi, 
				memMap->lenLow,
				MemoryTypeStrings[memMap->type -1]);

			mapIndex++;
		}

        printf("\nRegions initialised:\n    blocks: %i\n    used or reserved blocks: %i\n    free blocks: %i\n\n",
			memory::PhysicalMemoryManager::GetTotalBlockCount(), 
			memory::PhysicalMemoryManager::GetUsedBlockCount(), 
			memory::PhysicalMemoryManager::GetFreeBlockCount());
    }

    void KernelTests::Paging()
    {
        Terminal::ClearScreen();
        printf("Paging Test\n\n");
        printf("\nKernel Start 0x%x:\n", &s_KernelStart);
        printf("Paging Enabled: %s", memory::PhysicalMemoryManager::IsPagingEnabled() ? "true" : "false");
    }

    void KernelTests::Allocations()
    {
        Terminal::ClearScreen();
        printf("Allocations Test\n\n");

        uint32_t* p = (uint32_t*)memory::PhysicalMemoryManager::AllocateBlock ();
        printf ("p allocated at 0x%x\n", &p);

        uint32_t* p2 = (uint32_t*)memory::PhysicalMemoryManager::AllocateBlocks (2);
        printf ("allocated 2 blocks for p2 at 0x%x\n", &p2);

        memory::PhysicalMemoryManager::FreeBlock (p);
        p = (uint32_t*)memory::PhysicalMemoryManager::AllocateBlock  ();
        printf ("Unallocated p to free block 1. p is reallocated to 0x%x\n", &p);

        memory::PhysicalMemoryManager::FreeBlock (p);
        memory::PhysicalMemoryManager::FreeBlocks (p2, 2);
    }

    void KernelTests::SoftwareInterrupt()
    {
        hal_gen_interrupt();
    }
}