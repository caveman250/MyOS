#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <kernel/hal/HAL.h>
#include <kernel/VGA.h>
#include <kernel/Delay.h>
#include <kernel/Tests/KernelTests.h>
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
            if (++m_Row == s_VGAHeight)
            {
                m_Row = 0;
            }
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
        }

        if(m_HardwareCursorUpdatesEnabled)
            UpdateCursor();
    }

    void Terminal::WriteString(const char* data) 
    {
        Write(data, strlen(data));
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
		WriteString("\nCommand> ");

		hal::drivers::KeyCode key = hal::drivers::KeyCode::Unknown;
		bool BufChar;

		int i=0;
		while (i < n) 
		{
			BufChar = true;
			key = GetUserKeyCode();

			if (key == hal::drivers::KeyCode::Return)
			{
				//command entered, stop parsing
				break;
			}

			if (key == hal::drivers::KeyCode::BackSpace) 
			{
				//dont buffer this char
				BufChar = false;

				if (i > 0) 
				{
					size_t row, col;
					Terminal::GetCursorPos(row, col);

					if(col > 0)
					{
						col -= 1;
					}
					else 
					{
						row--;
						col = 80;
					}

					Terminal::SetCursorPos(row, col);
					Terminal::PutChar(' ');
					Terminal::SetCursorPos(row, col);

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
			WriteString("\nAvailable commands:\n");
			WriteString(" - exit: quit and pause the system\n");
			WriteString(" - clear: clear the screen\n");
			WriteString(" - help: display this message\n");
            WriteString(" - test: run a kernel\n");
		}
        else if(strcmp(cmd_buf, "test") == 0)
        {
            WriteString("\nInvalid test - enter test help for list of tests.\n");
        }
        else if(strcmp(cmd_buf, "test help") == 0)
        {
            WriteString("\nAvailable tests:\n");
			WriteString(" - memory_map: show the physical memory map provided by GRUB\n");
			WriteString(" - paging: show some info related to paging\n");
			WriteString(" - allocations: test physical memory allocations\n");
            WriteString(" - software_interrupt: throw an unhandled software interrupt\n");
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
		else 
		{
			WriteString("\nUnkown command");
		}

		return false;
    }
}

extern "C" void terminal_write(const char* data, size_t size)
{
    kernel::Terminal::Get().Write(data, size);
}