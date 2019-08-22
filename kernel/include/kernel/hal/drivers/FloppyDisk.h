#pragma once

#include <stdint.h>

namespace kernel::hal::drivers
{
    class FloppyDisk
    {
    public:
        static FloppyDisk& Get() { return s_Instance; }

        FloppyDisk();

        void Install(int irq);
        
        void SetWorkingDrive(uint8_t drive);
        uint8_t GetWorkingDrive();

        uint8_t* ReadSector(int sectorLBA);

        void LBAToCHS(int lba, int* head, int* track, int* sector);

        void HandleIrq();

    private:
        void InitialiseDirectMemoryAccess();
        void DirectMemoryAccessRead();
        void DirectMemoryAccessWrite();

        uint8_t ReadStatus();
        void WriteDigitalOutputRegister(uint8_t val);
        void SendCommand(uint8_t cmd);
        uint8_t ReadData();

        void WriteConfigControlRegister(uint8_t val);

        void WaitIrq();

        void CheckInterruptStatus(uint32_t* st0, uint32_t* cyl);
        void EnableMotor(bool enable);

        void SetupData(uint32_t stepRate, uint32_t loadTime, uint32_t unloadTime, bool dma);
        int Calibrate(uint32_t drive);

        void SetControllerEnabled(bool enabled);
        void Reset();

        void ReadSector_Impl(uint8_t head, uint8_t track, uint8_t sector);
        int Seek(uint32_t cyl, uint32_t head);

        uint8_t	m_CurrentDrive;
        volatile uint8_t m_FloppyDiskIRQ;

        enum class IO
        {
            DigitalOutputRegister = 0x3f2,
            MainStatusRegister = 0x3f4,
            DataRegister = 0x3f5,
            ControlRegister = 0x3f7
        };

        enum class Command
        {
            ReadTrack = 2,
            Specify = 3,
            CheckStat = 4,
            WriteSector = 5,
            ReadSector = 6,
            Calibrate = 7,
            CheckInt = 8,
            FormatTrack = 0xd,
            Seek = 0xf
        };

        enum class CommandExt
        {
            Skip = 0x20,
            Density = 0x40,
            MultiTrack = 0x80
        };

        enum class DigitalOutputRegisterMask
        {
            Drive0 = 0,
            Drive1 = 1,
            Drive2 = 2,
            Drive3 = 3,
            Reset = 4,
            DirectMemoryAccess = 8,
            Drive0Motor = 16,
            Drive1Motor = 32,
            Drive2Motor = 64,
            Drive3Motor = 128
        };

        enum class MainStatusRegisterMask
        {
            Drive0PosMode = 1,
            Drive1PosMode = 2,
            Drive2PosMode = 4,
            Drive3PosMode = 8,
            Busy = 16,
            DirectMemoryAccess = 32,
            DataIO = 64,
            DataRegister = 128
        };

        enum class StatusPort0Mask
        {
            Drive0 = 0,
            Drive1 = 1,
            Drive2 = 2,
            Drive3 = 3,
            HeadActive = 4,
            NotReady = 8,
            UnitCheck = 16,
            SeekEnd = 32,
            IntCode = 64
        };

        enum class StatusPort0IntCodeType
        {
            Normal = 0,
            AbnormalError = 1,
            InvalidError = 2,
            NotReady = 3
        };

        enum class Gap3Length
        {
            Standard = 42,
            FiveFourteen = 32,
            ThreeFive = 27
        };

        enum class BytesPerSector
        {
            Bytes128 = 0,
            Bytes256 = 1,
            Bytes512 = 2,
            Bytes1024 = 4
        };

        static constexpr int s_FloppyIrq = 6;
        static constexpr int s_SectorsPerTrack = 18;
        static constexpr int s_DirectMemoryAccessBuffer = 0x1000;

        static FloppyDisk s_Instance;
    };

    extern "C" void floppy_disk_handle_irq();
    extern "C" void floppy_disk_handle_irq_main();
}