#include <kernel/hal/IDT.h>
#include <kernel/hal/PIT.h>
#include <kernel/hal/PIC.h>
#include <kernel/hal/HAL.h>
#include <stdio.h>

namespace kernel
{
	#define		I86_PIT_REG_COUNTER0		0x40
	#define		I86_PIT_REG_COUNTER1		0x41
	#define		I86_PIT_REG_COUNTER2		0x42
	#define		I86_PIT_REG_COMMAND			0x43

	//Global Tick count
	static volatile uint32_t s_PitTicks=0;

	extern "C" void handle_irq_main() 
	{
		s_PitTicks++;

		HAL::InterruptFinished(0);
	}
	
	void PIT::Initialise()
	{
		//Install our interrupt handler (irq 0 uses interrupt 32)
		kernel::HAL::SetInterruptRoutine(32, (uint32_t)handle_irq);
	}

	uint32_t PIT::SetTickCount(uint32_t i)
	{
		uint32_t ret = s_PitTicks;
		s_PitTicks = i;
		return ret;
	}

	uint32_t PIT::GetTickCount()
	{
		return s_PitTicks;
	}

	void PIT::SendCommand(uint8_t cmd)
	{
		HAL::OutB(I86_PIT_REG_COMMAND, cmd);
	}

	void PIT::SendData(uint16_t data, uint8_t counter)
	{
		uint8_t port;
		switch(counter)
		{
			case I86_PIT_OCW_COUNTER_0:
				port = I86_PIT_REG_COUNTER0;
				break;
			case I86_PIT_OCW_COUNTER_1:
				port = I86_PIT_REG_COUNTER1;
				break;
			default:
				port = I86_PIT_REG_COUNTER2;
				break;
		}

		HAL::OutB(port, (uint8_t)data);
	}

	uint8_t PIT::ReadData(uint16_t counter) 
	{
		uint8_t port;
		switch(counter)
		{
			case I86_PIT_OCW_COUNTER_0:
				port = I86_PIT_REG_COUNTER0;
				break;
			case I86_PIT_OCW_COUNTER_1:
				port = I86_PIT_REG_COUNTER1;
				break;
			default:
				port = I86_PIT_REG_COUNTER2;
				break;
		}

		return HAL::InB(port);
	}

	void PIT::StartCounter(uint32_t freq, uint8_t counter, uint8_t mode)
	{
		if (freq == 0)
		{
			return;
		}

		uint16_t divisor = uint16_t(1193181 / (uint16_t)freq);

		//send operational command
		uint8_t ocw = 0;
		ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
		ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
		ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
		SendCommand(ocw);

		//set frequency rate
		SendData(divisor & 0xff, 0);
		SendData((divisor >> 8) & 0xff, 0);

		//reset tick count
		s_PitTicks = 0;
	}
}