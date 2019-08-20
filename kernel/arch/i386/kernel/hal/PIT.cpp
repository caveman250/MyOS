#include <kernel/hal/IDT.h>
#include <kernel/hal/PIT.h>
#include <kernel/hal/PIC.h>
#include <kernel/hal/HAL.h>
#include <stdio.h>

namespace kernel::hal
{
	PIT PIT::s_Instance;

	extern "C" void handle_irq_main() 
	{
		PIT::Get().IncrementTickCount();

		HAL::Get().InterruptFinished(0);
	}
	
	void PIT::Initialise()
	{
		//Install our interrupt handler (irq 0 uses interrupt 32)
		hal::HAL::Get().SetInterruptRoutine(32, (uint32_t)handle_irq);
	}

	void PIT::SetTickCount(uint32_t i)
	{
		m_PitTicks = i;
	}

	void PIT::IncrementTickCount()
	{
		++m_PitTicks;
	}

	uint32_t PIT::GetTickCount()
	{
		return m_PitTicks;
	}

	void PIT::SendCommand(uint8_t cmd)
	{
		hal::HAL::Get().OutB((unsigned short)ControlRegisters::Command, cmd);
	}

	void PIT::SendData(uint16_t data, uint8_t counter)
	{
		uint8_t port;
		switch(counter)
		{
			case (int)Counter::Counter0:
				port = (uint8_t)ControlRegisters::Counter0;
				break;
			case (int)Counter::Counter1:
				port = (uint8_t)ControlRegisters::Counter1;
				break;
			default:
				port = (uint8_t)ControlRegisters::Counter2;
				break;
		}

		hal::HAL::Get().OutB(port, (uint8_t)data);
	}

	uint8_t PIT::ReadData(uint16_t counter) 
	{
		uint8_t port;
		switch(counter)
		{
			case (int)Counter::Counter0:
				port = (uint8_t)ControlRegisters::Counter0;
				break;
			case (int)Counter::Counter1:
				port = (uint8_t)ControlRegisters::Counter1;
				break;
			default:
				port = (uint8_t)ControlRegisters::Counter2;
				break;
		}

		return hal::HAL::Get().InB(port);
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
		ocw = (ocw & ~(uint8_t)CommandMasks::Mode) | mode;
		ocw = (ocw & ~(uint8_t)CommandMasks::ReadLoad) | (uint8_t)TransferType::Data;
		ocw = (ocw & ~(uint8_t)CommandMasks::Counter) | counter;
		SendCommand(ocw);

		//set frequency rate
		SendData(divisor & 0xff, 0);
		SendData((divisor >> 8) & 0xff, 0);

		//reset tick count
		m_PitTicks = 0;
	}
}