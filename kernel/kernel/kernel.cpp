#include <stdio.h>
#include <string.h>

#include <vendor/multiboot.h>

#include <kernel/VGA.h>
#include <kernel/Terminal.h>
#include <kernel/hal/HAL.h>
#include <kernel/hal/drivers/Keyboard.h>
#include <kernel/Exception.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
#include <kernel/MemoryManagement/VirtualMemoryManager.h>

extern uint32_t kernelStart;
extern uint32_t kernelEnd;

namespace kernel
{
	void Init(unsigned int, multiboot_info_t* mbt) 
	{
		Terminal::Init();

		hal::HAL::Get().Initialise();

		hal::HAL::Get().SetInterruptRoutine(0, (uint32_t)Exception::DivideByZeroFault);
		hal::HAL::Get().SetInterruptRoutine(1, (uint32_t)Exception::SingleStepTrap);
		hal::HAL::Get().SetInterruptRoutine(2, (uint32_t)Exception::NmiTrap);
		hal::HAL::Get().SetInterruptRoutine(3, (uint32_t)Exception::BreakpointTrap);
		hal::HAL::Get().SetInterruptRoutine(4, (uint32_t)Exception::OverflowTrap);
		hal::HAL::Get().SetInterruptRoutine(5, (uint32_t)Exception::BoundsCheckFault);
		hal::HAL::Get().SetInterruptRoutine(6, (uint32_t)Exception::InvalidOpcodeFault);
		hal::HAL::Get().SetInterruptRoutine(7, (uint32_t)Exception::NoDeviceFault);
		hal::HAL::Get().SetInterruptRoutine(8, (uint32_t)Exception::DoubleFaultAbort);
		hal::HAL::Get().SetInterruptRoutine(10, (uint32_t)Exception::InvalidTssFault);
		hal::HAL::Get().SetInterruptRoutine(11, (uint32_t)Exception::NoSegmentFault);
		hal::HAL::Get().SetInterruptRoutine(12, (uint32_t)Exception::StackFault);
		hal::HAL::Get().SetInterruptRoutine(13, (uint32_t)Exception::GeneralProtectionFault);
		hal::HAL::Get().SetInterruptRoutine(14, (uint32_t)Exception::PageFault);
		hal::HAL::Get().SetInterruptRoutine(16, (uint32_t)Exception::FpuFault);
		hal::HAL::Get().SetInterruptRoutine(17, (uint32_t)Exception::AlignmentCheckFault);
		hal::HAL::Get().SetInterruptRoutine(18, (uint32_t)Exception::MachineCheckAbort);
		hal::HAL::Get().SetInterruptRoutine(19, (uint32_t)Exception::SimdFpuFault);

		printf("                          ___  ___        _____ _____ \n");
		printf("                          |  \\/  |       |  _  /  ___|\n");
		printf("                          | .  . |_   _  | | | \\ `--. \n");
		printf("                          | |\\/| | | | | | | | |`--. \\\n");
		printf("                          | |  | | |_| | \\ \\_/ /\\__/ /\n");
		printf("                          \\_|  |_/\\__, |  \\___/\\____/ \n");
		printf("                                   __/ |              \n");
		printf("                                  |___/               \n");

		int32_t memSize = 1024 + mbt->mem_lower + mbt->mem_upper;
		uint32_t kernelSize = &kernelEnd - &kernelStart;
		memory::PhysicalMemoryManager::Get().Initialise(memSize, (memory::physical_addr)&kernelEnd);

		int mapIndex = 0;
		for (multiboot_memory_map_t* memMap = (multiboot_memory_map_t*)mbt->mmap_addr; 
			(uint32_t)memMap < (uint32_t)(mbt->mmap_addr + mbt->mmap_length); 
			memMap++) 
		{
			mapIndex++;

			if(memMap->type == 1) //Available
			{
				memory::PhysicalMemoryManager::Get().InitialiseRegion(memMap->addrHi, memMap->lenHi);
			}
		}

		//deinit the region the kernel is in as its allready in use
		memory::PhysicalMemoryManager::Get().DeInitialiseRegion((memory::physical_addr)&kernelStart, kernelSize);

		memory::VirtualMemoryManager::Get().Initialise();

		hal::drivers::Keyboard::Get().Install(33);
	}

	extern "C" int kernel_main(unsigned int magic, multiboot_info_t* mbt)
	{
		Init(magic, mbt);
		printf("\nType help for a list of available commands.\n");
		Terminal::Run();

		Terminal::ClearScreen(VGA::CreateColour(VGA::Colour::LIGHT_GREY, VGA::Colour::BLACK));
		printf("\nexit recieved, pausing...");
		while(true);
		return 0;
	}
}
