#include <kernel/hal/drivers/FloppyDisk.h>
#include <kernel/hal/HAL.h>
#include <kernel/hal/DMAManager.h>
#include <kernel/Delay.h>

namespace kernel::hal::drivers
{
    FloppyDisk FloppyDisk::s_Instance;
    int FloppyDisk::s_DirectMemoryAccessBuffer = 0x1000;

    FloppyDisk::FloppyDisk()
        : m_CurrentDrive(0)
        , m_FloppyDiskIRQ(0)
    {

    }

     void FloppyDisk::Install(int irq)
     {
        HAL::Get().SetInterruptRoutine(irq, (uint32_t)floppy_disk_handle_irq);

        Reset();

        SetupData(13, 1, 0xf, true);
     }

             
    void FloppyDisk::SetWorkingDrive(uint8_t drive)
    {
        if (drive < 4)
        {
            m_CurrentDrive = drive;
        }
    }

    uint8_t FloppyDisk::GetWorkingDrive()
    {
        return m_CurrentDrive;
    }

    uint8_t* FloppyDisk::ReadSector(int sectorLBA)
    {
        if (m_CurrentDrive >= 4)
        {
            return 0;
        }

        int head = 0, track = 0, sector = 1;
        LBAToCHS(sectorLBA, &head, &track, &sector);

        EnableMotor(true);

        if(Seek(track, head) != 0)
        {
            return 0;
        }

        ReadSector_Impl(head, track, sector);
        EnableMotor(false);

        return (uint8_t*)s_DirectMemoryAccessBuffer;
    }

    void FloppyDisk::LBAToCHS(int lba, int* head, int* track, int* sector)
    {
        *head = (lba % (s_SectorsPerTrack * 2)) / s_SectorsPerTrack;
        *track = lba / (s_SectorsPerTrack * 2);
        *sector = lba % s_SectorsPerTrack + 1;
    }

    bool FloppyDisk::InitialiseDirectMemoryAccess(uint8_t* buffer, unsigned length)
    {
        union
        {
            uint8_t byte[4]; //Lo[0], Mid[1], Hi[2]
            unsigned long l;
        } a, c;
        
        a.l = (unsigned)buffer;
        c.l = (unsigned)length-1;

        DMAManager& dma = DMAManager::Get();
        dma.Reset();
        dma.MaskChannel(s_DirectMemoryAccessChannel);
        dma.ResetFlipFlop(1);

        dma.SetAddress(s_DirectMemoryAccessChannel, a.byte[0], a.byte[1]);
        dma.ResetFlipFlop(1);

        dma.SetCount(s_DirectMemoryAccessChannel, c.byte[0], c.byte[1]);
        dma.SetRead(s_DirectMemoryAccessChannel);

        dma.UnMaskAll();

        return true;
    }

    void FloppyDisk::SetDirectMemoryAccessBuffer(int addr)
    {
        s_DirectMemoryAccessBuffer = addr;
    }

    uint8_t FloppyDisk::ReadStatus()
    {
        return HAL::Get().InB((unsigned short)IO::MainStatusRegister);
    }

    void FloppyDisk::WriteDigitalOutputRegister(uint8_t val)
    {
        HAL::Get().OutB((unsigned short)IO::DigitalOutputRegister, val);
    }

    void FloppyDisk::SendCommand(uint8_t cmd)
    {
        for (int i = 0; i < 500; ++i)
        {
            if(ReadStatus() & (uint8_t)MainStatusRegisterMask::DataRegister)
            {
                HAL::Get().OutB((unsigned short)IO::DataRegister, cmd);
                return;
            }
        }
    }

    uint8_t FloppyDisk::ReadData()
    {
        for (int i = 0; i < 500; ++i)
        {
            if (ReadStatus() & (uint8_t)MainStatusRegisterMask::DataRegister)
            {
                return HAL::Get().InB((unsigned short)IO::DataRegister);
            }
        }

        return 0;
    }

    void FloppyDisk::WriteConfigControlRegister(uint8_t val)
    {
        HAL::Get().OutB((unsigned short)IO::ControlRegister, val);
    }

    void FloppyDisk::WaitIrq()
    {
        while (m_FloppyDiskIRQ == 0)
        {}

        m_FloppyDiskIRQ = 0;
    }

    void FloppyDisk::HandleIrq()
    {
        m_FloppyDiskIRQ = 1;
        HAL::Get().InterruptFinished(s_FloppyIrq);
    }

    void FloppyDisk::CheckInterruptStatus(uint32_t* st0, uint32_t* cyl)
    {
        SendCommand((uint8_t)Command::CheckInt);

        *st0 = ReadData();
        *cyl = ReadData();
    }

    void FloppyDisk::EnableMotor(bool enable)
    {
        if (m_CurrentDrive > 3)
        {
            //invalid drive
            return;
        }

        uint32_t motor = 0;

        switch (m_CurrentDrive)
        {
            case 0:
                motor = (uint32_t)DigitalOutputRegisterMask::Drive0Motor;
                break;
            case 1:
                motor = (uint32_t)DigitalOutputRegisterMask::Drive1Motor;
                break;
            case 2:
                motor = (uint32_t)DigitalOutputRegisterMask::Drive2Motor;
                break;
            case 3:
                motor = (uint32_t)DigitalOutputRegisterMask::Drive3Motor;
                break;
        }

        if (enable)
        {
            WriteDigitalOutputRegister(m_CurrentDrive | motor | (uint32_t)DigitalOutputRegisterMask::Reset | (uint32_t)DigitalOutputRegisterMask::DirectMemoryAccess);
        }
        else
        {
            WriteDigitalOutputRegister((uint32_t)DigitalOutputRegisterMask::Reset);
        }

        //wait a bit so the motor has time to start / stop
        Sleep(20);
    }

    void FloppyDisk::SetupData(uint32_t stepRate, uint32_t loadTime, uint32_t unloadTime, bool dma)
    {
        uint32_t data = 0;

        SendCommand((uint8_t)Command::Specify);

        data = ((stepRate & 0xf) << 4) | (unloadTime & 0xf);
        SendCommand(data);

        data = loadTime << 1 | (dma == false ? 0 : 1);
        SendCommand(data);
    }

    int FloppyDisk::Calibrate(uint32_t drive)
    {
        uint32_t st0, cyl;

        if (drive > 3)
        {
            return -2; //invalid drive
        }

        EnableMotor(true);

        for (int i = 0; i < 10; ++i)
        {
            SendCommand((uint8_t)Command::Calibrate);
            SendCommand(drive);
            WaitIrq();
            CheckInterruptStatus(&st0, &cyl);

            if(cyl == 0)
            {
                //found cylinder 0, done.
                EnableMotor(false);
                return 0;
            }
        }

        EnableMotor(false);
        return -1;
    }

    void FloppyDisk::SetControllerEnabled(bool enabled)
    {
        if (enabled)
        {
            WriteDigitalOutputRegister((uint8_t)DigitalOutputRegisterMask::Reset | (uint8_t)DigitalOutputRegisterMask::DirectMemoryAccess);
        }
        else
        {
            WriteDigitalOutputRegister(0);
        }
    }

    void FloppyDisk::Reset()
    {
        uint32_t st0, cyl;

        //reset the controller
        SetControllerEnabled(false);
        SetControllerEnabled(true);
        WaitIrq();
        
        //check interrupts for all drives.
        for (int i = 0; i < 4; ++i)
        {
            CheckInterruptStatus(&st0, &cyl);
        }

        WriteConfigControlRegister(0);

        SetupData(3, 16, 240, true);

        Calibrate(m_CurrentDrive);
    }

    void FloppyDisk::ReadSector_Impl(uint8_t head, uint8_t track, uint8_t sector)
    {
        uint32_t st0, cyl;

        InitialiseDirectMemoryAccess((uint8_t*)s_DirectMemoryAccessBuffer, 512);
        DMAManager::Get().SetRead(s_DirectMemoryAccessChannel);

        SendCommand((uint8_t)Command::ReadSector | (uint8_t)CommandExt::MultiTrack | (uint8_t)CommandExt::Skip | (uint8_t)CommandExt::Density);
        SendCommand(head << 2 | m_CurrentDrive);
        SendCommand(track);
        SendCommand(head);
        SendCommand(sector);
        SendCommand((uint8_t)BytesPerSector::Bytes512);
        SendCommand(((sector + 1) >= s_SectorsPerTrack) ? s_SectorsPerTrack : sector + 1);
        SendCommand((uint8_t)Gap3Length::ThreeFive);
        SendCommand(0xff);

        WaitIrq();

        for (int i = 0; i < 7; ++i)
        {
            ReadData();
        }

        CheckInterruptStatus(&st0, &cyl);
    }

    int FloppyDisk::Seek(uint32_t cyl, uint32_t head)
    {
        uint32_t st0, cyl0;

        if(m_CurrentDrive > 3)
        {
            return -1; //invalid drive
        }

        for (int i = 0; i < 10; ++i)
        {
            SendCommand((uint8_t)Command::Seek);
            SendCommand((head << 2 | m_CurrentDrive));
            SendCommand(cyl);

            WaitIrq();
            CheckInterruptStatus(&st0, &cyl0);

            //found cylinder?
            if(cyl0 == cyl)
            {
                return 0;
            }
        }

        return -1;
    }
}

extern "C" void floppy_disk_handle_irq_main()
{
    kernel::hal::drivers::FloppyDisk::Get().HandleIrq();
}