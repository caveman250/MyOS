#include <kernel/hal/CPU.h>
#include <kernel/hal/GDT.h>
#include <kernel/hal/IDT.h>

namespace kernel
{
	void CPU::Initialise () 
	{
		GDT::Initialise();
		IDT::Initialise(0x8);
	}

	void CPU::Shutdown () 
	{
	}

	const char* CPU::GetVendor()
	{
		return cpu_get_vendor();
	}
}
