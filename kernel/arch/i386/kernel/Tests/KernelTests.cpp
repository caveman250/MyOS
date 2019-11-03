#include <kernel/Tests/KernelTests.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <vendor/multiboot.h>

#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
#include <kernel/MemoryManagement/VirtualMemoryManager.h>
#include <kernel/hal/HAL.h>
#include <kernel/hal/drivers/FloppyDisk.h>
#include <kernel/Terminal.h>

extern uint32_t kernelStart;
extern uint32_t kernelEnd;


namespace kernel
{
    void KernelTests::RunTest(const char* testName)
    {
        if(strcmp(testName, "memory_map") == 0)
        {
            MemoryMap();
        }
        else if(strcmp(testName, "paging") == 0)
        {
            Paging();
        }
        else if(strcmp(testName, "allocations") == 0)
        {
            Allocations();
        }
        else if(strcmp(testName, "software_interrupt") == 0)
        {
            SoftwareInterrupt();
        }
        else if (strcmp(testName, "floppy_read_sector") == 0)
        {
            ReadFloppyDiskSector();
        }
        else if(strcmp(testName, "help") == 0)
        {
            ShowTestHelpMessage();
        }
        else
        {
            printf("\nInvalid test.");
            ShowTestHelpMessage();
        }
        
    }

    void KernelTests::MemoryMap()
    {
        printf("\nPhysical Memory Map Test\n\n");

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

        printf("\nRegions initialised:\n    blocks: %i\n    used or reserved blocks: %i\n    free blocks: %i\n",
			memory::PhysicalMemoryManager::Get().GetTotalBlockCount(), 
			memory::PhysicalMemoryManager::Get().GetUsedBlockCount(), 
			memory::PhysicalMemoryManager::Get().GetFreeBlockCount());
    }

    void KernelTests::Paging()
    {
        printf("\nPaging Test\n\n");
        printf("Paging Enabled: %s\n\n", memory::PhysicalMemoryManager::Get().IsPagingEnabled() ? "true" : "false");
        printf("Kernel Start should be 3GB (0xC0000000) more than Kernel Start Physical\n");
        printf("Kernel Start: 0x%x:\n", &kernelStart);
        printf("Kernel Start Physical: 0x%x\n", memory::VirtualMemoryManager::Get().GetPhysicalAddress((uint32_t)&kernelStart));
    }

    void KernelTests::Allocations()
    {
        printf("\nAllocations Test\n\n");

        uint32_t* p = (uint32_t*)memory::PhysicalMemoryManager::Get().AllocateBlock();
        printf("p allocated at 0x%x\n", &p);

        uint32_t* p2 = (uint32_t*)memory::PhysicalMemoryManager::Get().AllocateBlocks(2);
        printf("allocated 2 blocks for p2 at 0x%x\n", &p2);

        memory::PhysicalMemoryManager::Get().FreeBlock(p);
        p = (uint32_t*)memory::PhysicalMemoryManager::Get().AllocateBlock();
        printf("Unallocated p to free block 1. p is reallocated to 0x%x\n", &p);

        memory::PhysicalMemoryManager::Get().FreeBlock(p);
        memory::PhysicalMemoryManager::Get().FreeBlocks(p2, 2);
    }

    void KernelTests::SoftwareInterrupt()
    {
        hal_gen_interrupt();
    }

    void KernelTests::ReadFloppyDiskSector()
    {
        uint32_t sectorNum = 0;
        char sectorNumBuffer[4];
        uint8_t* sector = nullptr;

        printf("\nFloppyDisk Driver - Read Sector Test\n\n");

        printf("Enter sector number (default: 0)\n");
        Terminal::Get().GetUserCommand(sectorNumBuffer, 3);
        sectorNum = atoi(sectorNumBuffer);

        printf("\nSector %i contents:\n\n", sectorNum);

        sector = kernel::hal::drivers::FloppyDisk::Get().ReadSector(sectorNum);

        if(sector != 0)
        {
            int i = 0;
            for (int c = 0; c < 4; ++c)
            {
                for (int j = 0; j < 128; ++j)
                {
                    printf("0x%x ", sector[i + j]);
                }

                i += 128;
            }
        }
        else
        {
            printf("error reading disk\n");
        }
    }

    void KernelTests::ShowTestHelpMessage()
    {
        printf("\nAvailable tests:\n");
        printf(" - memory_map: show the physical memory map provided by GRUB\n");
        printf(" - paging: show some info related to paging\n");
        printf(" - allocations: test physical memory allocations\n");
        printf(" - software_interrupt: throw an unhandled software interrupt\n");
        printf(" - floppy_read_sector: read raw bytes from floppy disk drive\n");
    }
}