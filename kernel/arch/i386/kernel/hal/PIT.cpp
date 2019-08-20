#include <kernel/hal/IDT.h>
#include <kernel/hal/PIT.h>
#include <kernel/hal/PIC.h>
#include <kernel/hal/HAL.h>
#include <stdio.h>

namespace kernel
{
    enum class ControlRegisters
    {
        COUNTER_0 = 0x40,
        COUNTER_1 = 0x41,
        COUNTER_2 = 0x42,
        COMMAND = 0x43
    };

	//Global Tick count
	static volatile uint32_t s_PitTicks=0;

	extern "C" void handle_irq_main() 
	{
		s_PitTicks++;

		hal::HAL::InterruptFinished(0);
	}
	
	void PIT::Initialise()
	{
		//Install our interrupt handler (irq 0 uses interrupt 32)
		hal::HAL::SetInterruptRoutine(32, (uint32_t)handle_irq);
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
		hal::HAL::OutB((unsigned short)ControlRegisters::COMMAND, cmd);
	}

	void PIT::SendData(uint16_t data, uint8_t counter)
	{
		uint8_t port;
		switch(counter)
		{
			case (int)Counter::COUNTER_0:
				port = (uint8_t)ControlRegisters::COUNTER_0;
				break;
			case (int)Counter::COUNTER_1:
				port = (uint8_t)ControlRegisters::COUNTER_1;
				break;
			default:
				port = (uint8_t)ControlRegisters::COUNTER_2;
				break;
		}

		hal::HAL::OutB(port, (uint8_t)data);
	}

	uint8_t PIT::ReadData(uint16_t counter) 
	{
		uint8_t port;
		switch(counter)
		{
			case (int)Counter::COUNTER_0:
				port = (uint8_t)ControlRegisters::COUNTER_0;
				break;
			case (int)Counter::COUNTER_1:
				port = (uint8_t)ControlRegisters::COUNTER_1;
				break;
			default:
				port = (uint8_t)ControlRegisters::COUNTER_2;
				break;
		}

		return hal::HAL::InB(port);
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
		ocw = (ocw & ~(uint8_t)CommandMasks::MODE) | mode;
		ocw = (ocw & ~(uint8_t)CommandMasks::RL) | (uint8_t)TransferType::DATA;
		ocw = (ocw & ~(uint8_t)CommandMasks::COUNTER) | counter;
		SendCommand(ocw);

		//set frequency rate
		SendData(divisor & 0xff, 0);
		SendData((divisor >> 8) & 0xff, 0);

		//reset tick count
		s_PitTicks = 0;
	}
}