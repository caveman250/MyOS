#include <stdio.h>

#include <kernel/Terminal.h>
#include <kernel/hal/HAL.h>
#include <kernel/Exception.h>
 
int i = 100;

namespace kernel
{
	extern "C" void kernel_main(void) 
	{
		terminal::Init();
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

		printf("                          ___  ___        _____ _____ \n");
		printf("                          |  \\/  |       |  _  /  ___|\n");
		printf("                          | .  . |_   _  | | | \\ `--. \n");
		printf("                          | |\\/| | | | | | | | |`--. \\\n");
		printf("                          | |  | | |_| | \\ \\_/ /\\__/ /\n");
		printf("                          \\_|  |_/\\__, |  \\___/\\____/ \n");
		printf("                                   __/ |              \n");
		printf("                                  |___/               \n");

		printf("global variable 'i' as int:     [%i]\n", i);
		printf("global variable 'i' as hex:     [0x%x]\n", i);
		const char* str = "this is a const char*";
		printf("local variable 'str':           [%s]\n", str);
		printf("CPU Vendor: %s\n", HAL::GetCpuVendor());
		printf("Current tick count: %i\n", HAL::GetTickCount());
		printf("Press any key to throw an unhandled exception ;)\n");

		while(true)
		{
			size_t terminalRow, terminalCol;
			terminal::GetCursorPos(terminalRow, terminalCol);
			terminal::SetHardwareCursorUpdateEnabled(false);
			terminal::SetCursorPos(12, 0);

			printf("Current tick count: %i\n", HAL::GetTickCount());

			terminal::SetCursorPos(terminalRow, terminalCol);
			terminal::SetHardwareCursorUpdateEnabled(true);
		}
	}
}
