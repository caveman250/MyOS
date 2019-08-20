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
		hal::HAL::DisableInterrupts();

		va_list args;

		va_start(args, fmt);		
		va_end (args);

		const char* disclamer="***MyOS Kernel Panic***\nError Code Bellow:\n\n";

		Terminal::ClearScreen(VGA::CreateColour(VGA::Colour::WHITE, VGA::Colour::RED));
		Terminal::WriteString(disclamer);
		Terminal::WriteString(fmt);
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
		KernelPanic("General Protection Fault\n");
		unsigned bit0 = (1 << 0) & err;
		unsigned bit1 = (1 << 1) & err;
		unsigned bit2 = (1 << 2) & err;
		unsigned bit3 = (1 << 3) & err;
		unsigned bit4 = (1 << 4) & err;
		unsigned bit5 = (1 << 5) & err;
		unsigned bit6 = (1 << 6) & err;
		unsigned bit7 = (1 << 7) & err;
		unsigned bit8 = (1 << 8) & err;
		unsigned bit9 = (1 << 9) & err;
		unsigned bit10 = (1 << 10) & err;
		unsigned bit11 = (1 << 11) & err;
		unsigned bit12 = (1 << 12) & err;
		unsigned bit13 = (1 << 13) & err;
		unsigned bit14 = (1 << 14) & err;
		unsigned bit15 = (1 << 15) & err;
		printf("Error Code Bits 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7, bit8, bit9, bit10, bit11, bit12, bit13, bit14, bit15);
		printf("cs: 0x%x\nerr: 0x%x\neip: 0x%x\neflags: 0x%x", cs, err, eip, eflags);
		while(true);
	}

	void Exception::PageFault(unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags)
	{
		KernelPanic("Page Fault\n");
		unsigned bit0 = (1 << 0) & err;
		unsigned bit1 = (1 << 1) & err;
		unsigned bit2 = (1 << 2) & err;
		printf("%i %i %i", bit0, bit1, bit2);
		printf("%x %x %x", cs, eip, eflags);
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
