#include <kernel/Exception.h>
#include <kernel/hal/HAL.h>
#include <kernel/Terminal.h>
#include <kernel/VGA.h>
#include <stdarg.h>

namespace kernel
{
	void Exception::KernelPanic(const char* fmt, ...)
	{
		HAL::DisableInterrupts();

		va_list args;

		va_start(args, fmt);
		va_end (args);

		const char* disclamer="***MyOS Kernel Panic***\nError Code Bellow:\n\n";

		Terminal::ClearScreen(VGA::CreateColour(VGA::COLOUR_WHITE, VGA::COLOUR_RED));
		Terminal::WriteString(disclamer);
		Terminal::WriteString(fmt);

		while(true);
	}

	void Exception::DivideByZeroFault(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Divide by 0");
		while(true);
	}

	void Exception::SingleStepTrap(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Single step");
		while(true);
	}

	void Exception::NmiTrap(unsigned int cs, unsigned int eip, unsigned int eflags) 
	{
		KernelPanic("NMI trap");
		while(true);
	}

	void Exception::BreakpointTrap(unsigned int cs,unsigned int eip, unsigned int eflags) 
	{
		KernelPanic("Breakpoint trap");
		while(true);
	}

	void Exception::OverflowTrap(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Overflow trap");
		while(true);
	}

	void Exception::BoundsCheckFault(unsigned int cs, unsigned int eip, unsigned int eflags) 
	{
		KernelPanic("Bounds check fault");
		while(true);
	}

	void Exception::InvalidOpcodeFault(unsigned int cs, unsigned int eip, unsigned int eflags) 
	{
		KernelPanic("Invalid opcode");
		while(true);
	}

	void Exception::NoDeviceFault(unsigned int cs, unsigned int eip, unsigned int eflags) 
	{
		KernelPanic("Device not found");
		while(true);
	}

	void Exception::DoubleFaultAbort(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Double fault");
		while(true);
	}

	void Exception::InvalidTssFault(unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Invalid TSS");
		while(true);
	}

	void Exception::NoSegmentFault(unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Invalid segment");
		while(true);
	}

	void Exception::StackFault(unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Stack fault");
		while(true);
	}

	void Exception::GeneralProtectionFault(unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("General Protection Fault");
		while(true);
	}

	void Exception::PageFault(unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Page Fault");
		while(true);
	}

	void Exception::FpuFault(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("FPU Fault");
		while(true);
	}

	void Exception::AlignmentCheckFault(unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Alignment Check");
		while(true);
	}

	void Exception::MachineCheckAbort(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Machine Check");
		while(true);
	}

	void Exception::SimdFpuFault(unsigned int cs, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("FPU SIMD fault");
		while(true);
	}
}
