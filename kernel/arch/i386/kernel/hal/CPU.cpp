#include <kernel/hal/CPU.h>
#include <kernel/hal/GDT.h>
#include <kernel/hal/IDT.h>

namespace kernel::hal
{
	CPU CPU::s_Instance;

	void CPU::Initialise () 
	{
		GDT::Get().Initialise();
		IDT::Get().Initialise(0x8);
	}

	void CPU::Shutdown () 
	{
	}

	const char* CPU::GetVendor()
	{
		return cpu_get_vendor();
	}
}
