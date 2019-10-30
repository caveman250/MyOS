#include <kernel/hal/HAL.h>
#include <kernel/hal/DMAManager.h>

namespace kernel::hal
{
	DMAManager DMAManager::s_Instance;

	void DMAManager::MaskChannel(uint8_t channel)
	{
		if (channel <= 4)
		{
			HAL::Get().OutB((unsigned short)DMA0IO::ChannelMaskRegister, (1 << (channel-1)));
		}
		else
		{
			HAL::Get().OutB((unsigned short)DMA1IO::ChannelMaskRegister, (1 << (channel-5)));
		}
	}

	void DMAManager::UnMaskChannel(uint8_t channel)
	{
		if (channel <= 4)
		{
			HAL::Get().OutB((unsigned short)DMA0IO::ChannelMaskRegister, channel);
		}
		else
		{
			HAL::Get().OutB((unsigned short)DMA1IO::ChannelMaskRegister, channel);
		}
	}

	void DMAManager::UnMaskAll()
	{
		//it doesnt matter what is written to this register
		HAL::Get().OutB((unsigned short)DMA1IO::UnMaskAllRegister, 0xff);
	}

	void DMAManager::Reset()
	{
		//it doesnt matter what is written to this register
		HAL::Get().OutB((unsigned short)DMA0IO::TempRegister, 0xff);
	}

	void DMAManager::ResetFlipFlop(int dma)
	{
		if (dma >= 2)
			return;

		//it doesnt matter what is written to this register
		HAL::Get().OutB(dma == 0 ? (unsigned short)DMA0IO::ClearByteFlipFlipRegister : (unsigned short)DMA1IO::ClearByteFlipFlipRegister, 0xff);
	}

	void DMAManager::SetAddress(uint8_t channel, uint8_t low, uint8_t high) 
	{
		if (channel > 8)
		{
			return;
		}

		unsigned short port = 0;
		switch (channel) 
		{
			case 0: port = (unsigned short)DMA0ChannelIO::Channel0AddressRegister; break;
			case 1: port = (unsigned short)DMA0ChannelIO::Channel1AddressRegister; break;
			case 2: port = (unsigned short)DMA0ChannelIO::Channel2AddressRegister; break;
			case 3: port = (unsigned short)DMA0ChannelIO::Channel3AddressRegister; break;
			case 4: port = (unsigned short)DMA1ChannelIO::Channel4AddressRegister; break;
			case 5: port = (unsigned short)DMA1ChannelIO::Channel5AddressRegister; break;
			case 6: port = (unsigned short)DMA1ChannelIO::Channel6AddressRegister; break;
			case 7: port = (unsigned short)DMA1ChannelIO::Channel7AddressRegister; break;
		}

		HAL::Get().OutB(port, low);
		HAL::Get().OutB(port, high);
	}

	void DMAManager::SetCount(uint8_t channel, uint8_t low, uint8_t high)
	{
		if (channel > 8)
		{
			return;
		}

		unsigned short port = 0;
		switch ( channel ) 
		{
			case 0: port = (unsigned short)DMA0ChannelIO::Channel0CountRegister; break;
			case 1: port = (unsigned short)DMA0ChannelIO::Channel1CountRegister; break;
			case 2: port = (unsigned short)DMA0ChannelIO::Channel2CountRegister; break;
			case 3: port = (unsigned short)DMA0ChannelIO::Channel3CountRegister; break;
			case 4: port = (unsigned short)DMA1ChannelIO::Channel4CountRegister; break;
			case 5: port = (unsigned short)DMA1ChannelIO::Channel5CountRegister; break;
			case 6: port = (unsigned short)DMA1ChannelIO::Channel6CountRegister; break;
			case 7: port = (unsigned short)DMA1ChannelIO::Channel7CountRegister; break;
		}

		HAL::Get().OutB(port, low);
		HAL::Get().OutB(port, high);
	}

	void DMAManager::SetMode(uint8_t channel, uint8_t mode)
	{
		int dma = channel < 4 ? 0 : 1;
		int chan = dma == 0 ? channel : channel - 4;

		MaskChannel(channel);
		HAL::Get().OutB(channel < 4 ? (unsigned short)DMA0IO::ModeRegister : (unsigned short)DMA1IO::ModeRegister, chan | mode);
		UnMaskAll();
	}

	void DMAManager::SetRead(uint8_t channel)
	{
		SetMode(channel, (unsigned short)DMAModeRegisterMask::ReadTransfer | (unsigned short)DMAModeRegisterMask::TransferSingle);
	}

	void DMAManager::SetWrite(uint8_t channel)
	{
		SetMode(channel, (unsigned short)DMAModeRegisterMask::WriteTransfer | (unsigned short)DMAModeRegisterMask::TransferSingle);
	}

	void DMAManager::SetExternalPageRegister(uint8_t reg, uint8_t val)
	{
		if (reg > 14)
		{
			return;
		}

		unsigned short port = 0;
		switch (reg) 
		{
			case 1: port = (unsigned short)DMA0PageRegister::Channel1AddressByte2; break;
			case 2: port = (unsigned short)DMA0PageRegister::Channel2AddressByte2; break;
			case 3: port = (unsigned short)DMA0PageRegister::Channel3AddressByte2; break;
			case 4:  return; //nothing should ever write to register 4
			case 5: port = (unsigned short)DMA0PageRegister::Channel5AddressByte2; break;
			case 6: port = (unsigned short)DMA0PageRegister::Channel6AddressByte2; break;
			case 7: port = (unsigned short)DMA0PageRegister::Channel7AddressByte2; break;
		}

		HAL::Get().OutB(port, val);
	}
}
