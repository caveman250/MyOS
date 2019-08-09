#include <kernel/hal/HAL.h>
#include <kernel/hal/CPU.h>

namespace kernel
{
	void HAL::Initialise () 
	{
		CPU::Initialise();
	}

	void HAL::Shutdown ()
	{
		CPU::Shutdown ();
	}
}