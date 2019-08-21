#include <kernel/Exception.h>
#include <kernel/hal/HAL.h>
#include <kernel/Terminal.h>
#include <kernel/VGA.h>
#include <stdarg.h>
#include <stdio.h>

namespace kernel
{
	void Exception::KernelPanic(const char* fmt, ...)
	{
		//TODO need a way of printing the args, should implement vsnprintf
		hal::HAL::Get().DisableInterrupts();

		va_list args;

		va_start(args, fmt);		
		va_end (args);

		const char* disclamer="***MyOS Kernel Panic***\nError Code Bellow:\n\n";

		Terminal::Get().ClearScreen(VGA::CreateColour(VGA::Colour::WHITE, VGA::Colour::RED));
		Terminal::Get().WriteString(disclamer);
		Terminal::Get().WriteString(fmt);
	}

	void Exception::DivideByZeroFault(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Divide by 0");
		while(true);
	}

	void Exception::SingleStepTrap(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Single step");
		while(true);
	}

	void Exception::NmiTrap(unsigned int, unsigned int, unsigned int) 
	{
		KernelPanic("NMI trap");
		while(true);
	}

	void Exception::BreakpointTrap(unsigned int,unsigned int, unsigned int) 
	{
		KernelPanic("Breakpoint trap");
		while(true);
	}

	void Exception::OverflowTrap(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Overflow trap");
		while(true);
	}

	void Exception::BoundsCheckFault(unsigned int, unsigned int, unsigned int) 
	{
		KernelPanic("Bounds check fault");
		while(true);
	}

	void Exception::InvalidOpcodeFault(unsigned int, unsigned int, unsigned int) 
	{
		KernelPanic("Invalid opcode");
		while(true);
	}

	void Exception::NoDeviceFault(unsigned int, unsigned int, unsigned int) 
	{
		KernelPanic("Device not found");
		while(true);
	}

	void Exception::DoubleFaultAbort(unsigned int, unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Double fault");
		while(true);
	}

	void Exception::InvalidTssFault(unsigned int, unsigned int,  unsigned int, unsigned int)
	{
		KernelPanic("Invalid TSS");
		while(true);
	}

	void Exception::NoSegmentFault(unsigned int, unsigned int,  unsigned int, unsigned int)
	{
		KernelPanic("Invalid segment");
		while(true);
	}

	void Exception::StackFault(unsigned int, unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Stack fault");
		while(true);
	}

	void Exception::GeneralProtectionFault(unsigned int, unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("General Protection Fault\n");
		while(true);
	}

	void Exception::PageFault(unsigned int, unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Page Fault\n");
		while(true);
	}

	void Exception::FpuFault(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("FPU Fault");
		while(true);
	}

	void Exception::AlignmentCheckFault(unsigned int,unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Alignment Check");
		while(true);
	}

	void Exception::MachineCheckAbort(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("Machine Check");
		while(true);
	}

	void Exception::SimdFpuFault(unsigned int, unsigned int, unsigned int)
	{
		KernelPanic("FPU SIMD fault");
		while(true);
	}
}
