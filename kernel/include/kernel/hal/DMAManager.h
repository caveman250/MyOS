#pragma once

#if !defined(__i386__)
#pragma error "HAL not implimented for this platform"
#endif

#include <stdint.h>

namespace kernel::hal
{
    static constexpr int s_MaxDMAChannels = 16;
    static constexpr int s_DMAChannelsPerDMAC = 8;

    class DMAManager
    {
    public:
        static DMAManager& Get() { return s_Instance; }

        void SetMode(uint8_t channel, uint8_t mode);
        void SetRead(uint8_t channel);
        void SetWrite(uint8_t channel);
        void SetAddress(uint8_t channel, uint8_t low, uint8_t high);
        void SetCount(uint8_t channel, uint8_t low, uint8_t high);
        void MaskChannel(uint8_t channel);
        void UnMaskChannel(uint8_t channel);
        void ResetFlipFlop(int dma);
        void Reset();
        void SetExternalPageRegister(uint8_t reg, uint8_t val);
        void UnMaskAll();

    private:
        static DMAManager s_Instance;

        enum class DMA0ChannelIO 
        {
            Channel0AddressRegister = 0,
            Channel0CountRegister = 1,
            Channel1AddressRegister = 2,
            Channel1CountRegister = 3,
            Channel2AddressRegister = 4,
            Channel2CountRegister = 5,
            Channel3AddressRegister = 6,
            Channel3CountRegister = 7
        };

        enum class DMA0IO 
        {
            StatusRegister = 0x08,
            CommandRegister = 0x08,
            RequestRegister = 0x09,
            ChannelMaskRegister = 0x0a,
            ModeRegister = 0x0b,
            ClearByteFlipFlipRegister = 0x0c,
            TempRegister = 0x0d,
            MasterClearRegister = 0x0d,
            ClearMaskRegister = 0x0e,
            MaskRegister = 0x0f
        };

        enum class DMA0PageRegister 
        {
            Extra0 = 0x80, //! Also diagnostics port
            Channel2AddressByte2 = 0x81,
            Channel3AddressByte2 = 0x82,
            Channel1AddressByte2 = 0x83,
            PageExtra1 = 0x84,
            PageExtra2 = 0x85,
            PageExtra3 = 0x86,
            Channel6AddressByte2 = 0x87,
            Channel7AddressByte2 = 0x88,
            Channel5AddressByte2 = 0x89,
            PageExtra4 = 0x8c,
            PageExtra5 = 0x8d,
            PageExtra6 = 0x8e,
            PageDRAMRefresh = 0x8f //no longer used in new PCs
        };

        enum class DMA1ChannelIO 
        {
            Channel4AddressRegister = 0xc0,
            Channel4CountRegister = 0xc2,
            Channel5AddressRegister = 0xc4,
            Channel5CountRegister = 0xc6,
            Channel6AddressRegister = 0xc8,
            Channel6CountRegister = 0xca,
            Channel7AddressRegister = 0xcc,
            Channel7CountRegister = 0xce,
        };

        enum class DMA1IO 
        {
            StatusRegister = 0xd0,
            CommandRegister = 0xd0,
            RequestRegister = 0xd2,
            ChannelMaskRegister = 0xd4,
            ModeRegister = 0xd6,
            ClearByteFlipFlipRegister = 0xd8,
            InterRegister = 0xda,
            UnMaskAllRegister = 0xdc,
            MaskRegister = 0xde
        };

        enum class DMAModeRegisterMask 
        {
            MaskSelect = 3,

            MaskTRA = 0xc,
            SelfTest = 0,
            ReadTransfer =4,
            WriteTransfer = 8,

            MaskAuto = 0x10,
            MaskIDEC = 0x20,

            ModeMask = 0xc0,
            TransferOnDemand = 0,
            TransferSingle = 0x40,
            TransferBlock = 0x80,
            TransferCascade = 0xC0
        };

        enum class DMACommandRegisterMask 
        {
            MemToMem = 1,
            Channel0AddressHold = 2,
            Enable = 4,
            Timing = 8,
            Priority = 0x10,
            WriteSelector = 0x20,
            DREQ = 0x40,
            DACK = 0x80
        };

    };
}