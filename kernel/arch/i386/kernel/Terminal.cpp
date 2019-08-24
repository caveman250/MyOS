#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <kernel/hal/HAL.h>
#include <kernel/VGA.h>
#include <kernel/Delay.h>
#include <kernel/Tests/KernelTests.h>

#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
namespace kernel
{
    Terminal Terminal::s_Instance;

    Terminal::Terminal()
        : m_SerialPort(hal::SerialPort::SerialPortAddress::Com1)
        , m_Buffer(nullptr)
        , m_Row(0)
        , m_Column(0)
        , m_Colour(0)
        , m_HardwareCursorUpdatesEnabled(true)
    {
        for(int i = 0; i < s_CommandHistorySize; ++i)
        {
            m_CommandHistory[i] = nullptr;
        }
    }

    void Terminal::Initialise() 
    {
        m_Buffer = (uint16_t*)0x000B8000;
        
        ClearScreen(VGA::CreateColour(VGA::Colour::LIGHT_GREY, VGA::Colour::BLUE));
        SetHardwareCursorUpdateEnabled(true);
    }

    void Terminal::InitialiseSerialPort()
    {
        m_SerialPort.Initialise();
    }

    void Terminal::PutEntryAt(char c, uint8_t color, size_t x, size_t y) 
    {
        const size_t index = y * s_VGAWidth + x;
        m_Buffer[index] = VGA::Entry(c, color);
    }

    void Terminal::PutChar(char c) 
    {
        m_SerialPort.Write(c);
        
        if(c == '\n')
        {
            m_Row++;
            m_Column = 0;
            return;
        }

        PutEntryAt(c, m_Colour, m_Column, m_Row);

       if (++m_Column == s_VGAWidth) 
       {
           m_Column = 0;
           ++m_Row;
              ScrollIfNecessary();
       }
    }

    void Terminal::UpdateCursor()
    {
        uint16_t pos = m_Row * s_VGAWidth + m_Column;
    
        hal::HAL::Get().OutB(0x3D4, 0x0F);
        hal::HAL::Get().OutB(0x3D5, (uint8_t)(pos & 0xFF));
        hal::HAL::Get().OutB(0x3D4, 0x0E);
        hal::HAL::Get().OutB(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    }

    void Terminal::Write(const char* data, size_t size) 
    {
        for (size_t i = 0; i < size; i++)
        {
            PutChar(data[i]);
            ScrollIfNecessary();
        }

        if(m_HardwareCursorUpdatesEnabled)
            UpdateCursor();
    }

    void Terminal::WriteString(const char* data) 
    {
        Write(data, strlen(data));
    }

    void Terminal::ScrollIfNecessary()
    {
        if (m_Row >= s_VGAHeight)
        {
            //prune the top row of the buffer
            for (size_t y = 0; y < s_VGAHeight; y++) 
            {
                for (size_t x = 0; x < s_VGAWidth; x++) 
                {
                    const size_t index = y * s_VGAWidth + x;
                    const size_t newValueIndex = (y + 1) * s_VGAWidth + x;
                    if (y < s_VGAHeight -1)
                    {
                        m_Buffer[index] = m_Buffer[newValueIndex];
                    }
                    else
                    {
                        m_Buffer[index] = VGA::Entry(' ', m_Colour);
                    }
                }
            }

            m_Row = s_VGAHeight - 1;
        }
    }

    void Terminal::RemoveLastInputCharacter()
    {
        size_t row, col;
        GetCursorPos(row, col);

        if(col > 0)
        {
            col -= 1;
        }
        else 
        {
            row--;
            col = 80;
        }

        SetCursorPos(row, col);
        PutChar(' ');
        SetCursorPos(row, col);
    }

    void Terminal::ClearScreen(uint8_t colour)
    {
        if(colour != 0)
        {
            m_Colour = colour;
        }

        for (size_t y = 0; y < s_VGAHeight; y++) 
        {
            for (size_t x = 0; x < s_VGAWidth; x++) 
            {
                const size_t index = y * s_VGAWidth + x;
                m_Buffer[index] = VGA::Entry(' ', m_Colour);
            }
        }

        m_Row = 0;
        m_Column = 0;
        UpdateCursor();
        SetHardwareCursorUpdateEnabled(true);
    }

    void Terminal::SetCursorPos(size_t row, size_t col)
    {
        m_Row = row;
        m_Column = col;

        if(m_HardwareCursorUpdatesEnabled)
        {
            UpdateCursor();
        }
    }

    void Terminal::GetCursorPos(size_t& row, size_t& col)
    {
        row = m_Row;
        col = m_Column;
    }

    void Terminal::SetHardwareCursorUpdateEnabled(bool enabled)
    {
        m_HardwareCursorUpdatesEnabled = enabled;
    }
    
    void Terminal::Run()
    {
		char buffer[100];

		while (true) 
		{
            WriteString("\nCommand> ");
			GetUserCommand(buffer, 98);

			if (RunUserCommand(buffer) == true)
				break;
		}
    }

    hal::drivers::KeyCode Terminal::GetUserKeyCode()
    {
		hal::drivers::KeyCode key = hal::drivers::KeyCode::Unknown;

		//wait for input
		while(key == hal::drivers::KeyCode::Unknown)
        {
			key = hal::drivers::Keyboard::Get().GetLastKeyCode();
        }

		//key press is handled by the idt, assume it was handled by this point since we didnt throw an exception.
		hal::drivers::Keyboard::Get().DiscardLastKeyCode();
		return key;
    }

    void Terminal::GetUserCommand(char* buf, int n)
    {
        using namespace hal::drivers;

		KeyCode key = KeyCode::Unknown;
		bool BufChar;

		int i=0;
		while (i < n) 
		{
			BufChar = true;
			key = GetUserKeyCode();

			if (key == KeyCode::Return)
			{
				//command entered, stop parsing
				break;
			}
            else if (key == KeyCode::Up || key == KeyCode::Down)
            {
                BufChar = false;
                
                while(i > 0)
                {
                    RemoveLastInputCharacter();
                    i--;
                }

                if (key == KeyCode::Up)
                {
                    m_CommandHistoryIndex = m_CommandHistoryIndex - 1 >= 0 ? m_CommandHistoryIndex - 1 : s_CommandHistorySize - 1;
                }
                else
                {
                    m_CommandHistoryIndex = m_CommandHistoryIndex + 1 <= s_CommandHistorySize - 1 ? m_CommandHistoryIndex + 1 : 0;
                }

                if (m_CommandHistory[m_CommandHistoryIndex] != nullptr)
                {
                    char* nextCommandHistoryValue = m_CommandHistory[m_CommandHistoryIndex];
                    int len = strlen(nextCommandHistoryValue);

                    for (int j = 0; j < len; ++j)
                    {
                        char val = nextCommandHistoryValue[j];
                        PutChar(val);
                        buf[i++] = val;
                    }
                }

                UpdateCursor();
            }
			else if (key == KeyCode::BackSpace) 
			{
				//dont buffer this char
				BufChar = false;

				if (i > 0) 
				{
                    RemoveLastInputCharacter();
					i--;
				}
			}

			if (BufChar) 
			{
				char c = hal::drivers::Keyboard::Get().KeyCodeToAscii(key);
				if (c != 0) 
				{
					Terminal::PutChar(c);
					Terminal::UpdateCursor();
					buf [i++] = c;
				}
			}

			Sleep(5);
		}

		//null terminate the string
		buf [i] = '\0';
    }

    bool Terminal::RunUserCommand(char* cmd_buf)
    {
        m_CommandHistory[m_CommandHistoryInsertIndex] = (char*)memory::PhysicalMemoryManager::Get().AllocateBlock();
        memcpy(m_CommandHistory[m_CommandHistoryInsertIndex], cmd_buf, strlen(cmd_buf) + 1);
        m_CommandHistoryIndex = m_CommandHistoryInsertIndex + 1;
        m_CommandHistoryInsertIndex = (m_CommandHistoryInsertIndex + 1) % s_CommandHistorySize;

        if (strcmp(cmd_buf, "exit") == 0) 
		{
			return true;
		}
		else if (strcmp(cmd_buf, "clear") == 0) 
		{
			Terminal::ClearScreen(VGA::CreateColour(VGA::Colour::LIGHT_GREY, VGA::Colour::BLUE));
		}
		else if(strcmp (cmd_buf, "help") == 0)
		{
			ShowHelpMessage();
		}
        else if(strcmp(cmd_buf, "test") == 0)
        {
            WriteString("\nInvalid test\n");
            ShowTestHelpMessage();
        }
        else if(strcmp(cmd_buf, "test help") == 0)
        {
            ShowTestHelpMessage();
        }
        else if(strcmp(cmd_buf, "test memory_map") == 0)
        {
            KernelTests::MemoryMap();
        }
        else if(strcmp(cmd_buf, "test paging") == 0)
        {
            KernelTests::Paging();
        }
        else if(strcmp(cmd_buf, "test allocations") == 0)
        {
            KernelTests::Allocations();
        }
        else if(strcmp(cmd_buf, "test software_interrupt") == 0)
        {
            KernelTests::SoftwareInterrupt();
        }
        else if (strcmp(cmd_buf, "test floppy_read_sector") == 0)
        {
            KernelTests::ReadFloppyDiskSector();
        }
		else 
		{
			WriteString("\nUnkown command");
		}

		return false;
    }

    void Terminal::ShowHelpMessage()
    {
        WriteString("\nAvailable commands:\n");
        WriteString(" - exit: quit and pause the system\n");
        WriteString(" - clear: clear the screen\n");
        WriteString(" - help: display this message\n");
        WriteString(" - test: run a kernel test\n");
    }

    void Terminal::ShowTestHelpMessage()
    {
        WriteString("\nAvailable tests:\n");
        WriteString(" - memory_map: show the physical memory map provided by GRUB\n");
        WriteString(" - paging: show some info related to paging\n");
        WriteString(" - allocations: test physical memory allocations\n");
        WriteString(" - software_interrupt: throw an unhandled software interrupt\n");
    }
}

extern "C" void terminal_write(const char* data, size_t size)
{
    kernel::Terminal::Get().Write(data, size);
}