#include <kernel/hal/HAL.h>

namespace kernel
{
    static inline void Sleep(int ms) 
	{
		int ticks = ms + hal::HAL::GetTickCount();
		while (ticks > hal::HAL::GetTickCount());
	}
}