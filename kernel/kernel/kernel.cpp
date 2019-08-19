#include <stdio.h>

#include <vendor/multiboot.h>

#include <kernel/Terminal.h>
#include <kernel/hal/HAL.h>
#include <kernel/Exception.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
#include <kernel/MemoryManagement/VirtualMemoryManager.h>

const char* MemoryTypeStrings[] = {
	"Available",
	"Reserved",
	"ACPI Reclaim",
	"ACPI NVS Memory",
	"Bad Ram"
};

extern uint32_t kernelStart;
extern uint32_t kernelEnd;

namespace kernel
{
	extern "C" void kernel_main(unsigned int magic, multiboot_info_t* mbt) 
	{
		int integer = 100;
		Terminal::Init();
		HAL::Initialise();

		HAL::SetInterruptRoutine(0, (uint32_t)Exception::DivideByZeroFault);
		HAL::SetInterruptRoutine(1, (uint32_t)Exception::SingleStepTrap);
		HAL::SetInterruptRoutine(2, (uint32_t)Exception::NmiTrap);
		HAL::SetInterruptRoutine(3, (uint32_t)Exception::BreakpointTrap);
		HAL::SetInterruptRoutine(4, (uint32_t)Exception::OverflowTrap);
		HAL::SetInterruptRoutine(5, (uint32_t)Exception::BoundsCheckFault);
		HAL::SetInterruptRoutine(6, (uint32_t)Exception::InvalidOpcodeFault);
		HAL::SetInterruptRoutine(7, (uint32_t)Exception::NoDeviceFault);
		HAL::SetInterruptRoutine(8, (uint32_t)Exception::DoubleFaultAbort);
		HAL::SetInterruptRoutine(10, (uint32_t)Exception::InvalidTssFault);
		HAL::SetInterruptRoutine(11, (uint32_t)Exception::NoSegmentFault);
		HAL::SetInterruptRoutine(12, (uint32_t)Exception::StackFault);
		HAL::SetInterruptRoutine(13, (uint32_t)Exception::GeneralProtectionFault);
		HAL::SetInterruptRoutine(14, (uint32_t)Exception::PageFault);
		HAL::SetInterruptRoutine(16, (uint32_t)Exception::FpuFault);
		HAL::SetInterruptRoutine(17, (uint32_t)Exception::AlignmentCheckFault);
		HAL::SetInterruptRoutine(18, (uint32_t)Exception::MachineCheckAbort);
		HAL::SetInterruptRoutine(19, (uint32_t)Exception::SimdFpuFault);

		//printf("                          ___  ___        _____ _____ \n");
		//printf("                          |  \\/  |       |  _  /  ___|\n");
		//printf("                          | .  . |_   _  | | | \\ `--. \n");
		//printf("                          | |\\/| | | | | | | | |`--. \\\n");
		//printf("                          | |  | | |_| | \\ \\_/ /\\__/ /\n");
		//printf("                          \\_|  |_/\\__, |  \\___/\\____/ \n");
		//printf("                                   __/ |              \n");
		//printf("                                  |___/               \n");

		//printf("0x%x\n", mbt);
		//printf("0x%x\n", &kernelStart);
		//printf("mbt magic 0x%x:\n", magic);
		

		//printf("mem lower: 0x%x\n", mbt->mem_lower);
		//printf("mem upper: 0x%x\n", mbt->mem_upper);

		int32_t memSize = 1024 + mbt->mem_lower + mbt->mem_upper;

		//printf("Memory Size 0x%x\n", memSize);
		uint32_t kernelSize = &kernelEnd - &kernelStart;
		printf("Kernel Size 0x%x:\n", kernelSize);
		printf("Kernel Start 0x%x:\n", &kernelStart);
		printf("Kernel End 0x%x:\n", &kernelEnd);
		printf("Calculated kernel end 0x%x\n", &kernelStart + kernelSize);
		printf("Calculated kernel end 2 0x%x\n", (uint32_t*)0xC0100000 + kernelSize);

		memory::PhysicalMemoryManager::Initialise(memSize, (memory::physical_addr)&kernelEnd);

		printf("mbt size 0x%x\n", mbt->mmap_length);
		printf("\nPhysical Memory Map:\n");
		int mapIndex = 0;
		printf("0x%x\n", (uint32_t)(mbt->mmap_addr + mbt->mmap_length));
		for (multiboot_memory_map_t* memMap = (multiboot_memory_map_t*)mbt->mmap_addr; 
			(uint32_t)memMap < (uint32_t)(mbt->mmap_addr + mbt->mmap_length); 
			memMap++) 
		{
			printf("    region %i: start: 0x%x%x length bytes: 0x%x%x type: %s\n", 
				mapIndex, 
				memMap->addrHi, 
				memMap->addrLow,
				memMap->lenHi, 
				memMap->lenLow,
				MemoryTypeStrings[memMap->type -1]);

			mapIndex++;

			if(memMap->type == 1 /* Available */)
			{
				memory::PhysicalMemoryManager::InitialiseRegion(memMap->addrHi, memMap->lenHi);
			}
		}

		//deinit the region the kernel is in as its allready in use
		memory::PhysicalMemoryManager::DeInitialiseRegion((memory::physical_addr)&kernelStart, kernelSize);

		printf("\nRegions initialised:\n    blocks: %i\n    used or reserved blocks: %i\n    free blocks: %i\n\n",
			memory::PhysicalMemoryManager::GetTotalBlockCount(), 
			memory::PhysicalMemoryManager::GetUsedBlockCount(), 
			memory::PhysicalMemoryManager::GetFreeBlockCount());

		memory::VirtualMemoryManager::Initialise();

		printf("Paging Enabled: %s", memory::PhysicalMemoryManager::IsPagingEnabled() ? "true" : "false");

		//uint32_t* p = (uint32_t*)PhysicalMemoryManager::AllocateBlock ();
		//printf ("\np allocated at 0x%x", &p);
//
		//uint32_t* p2 = (uint32_t*)PhysicalMemoryManager::AllocateBlocks (2);
		//printf ("\nallocated 2 blocks for p2 at 0x%x", &p2);
//
		//PhysicalMemoryManager::FreeBlock (p);
		//p = (uint32_t*)PhysicalMemoryManager::AllocateBlock  ();
		//printf ("\nUnallocated p to free block 1. p is reallocated to 0x%x", &p);
//
		//PhysicalMemoryManager::FreeBlock (p);
		//PhysicalMemoryManager::FreeBlocks (p2, 2);

		printf("\nPress any key to throw an unhandled exception ;)");

		while(true)
		{
		}
	}
}
