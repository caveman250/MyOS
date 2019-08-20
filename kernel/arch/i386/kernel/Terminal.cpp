#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <kernel/hal/HAL.h>
#include <kernel/VGA.h>
#include <kernel/Delay.h>

namespace kernel
{
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;

    static size_t s_Row;
    static size_t s_Column;
    static uint8_t s_Colour;
    static uint16_t* s_Buffer;
    static bool s_HardwareCursorUpdatesEnabled;

    void Terminal::Init() 
    {
        s_Buffer = (uint16_t*) 0x000B8000;
        
        ClearScreen(VGA::CreateColour(VGA::Colour::LIGHT_GREY, VGA::Colour::BLUE));
        SetHardwareCursorUpdateEnabled(true);
    }

    void Terminal::PutEntryAt(char c, uint8_t color, size_t x, size_t y) 
    {
        const size_t index = y * VGA_WIDTH + x;
        s_Buffer[index] = VGA::Entry(c, color);
    }

    void Terminal::PutChar(char c) 
    {
        if(c == '\n')
        {
            s_Row++;
            s_Column = 0;
            return;
        }

        PutEntryAt(c, s_Colour, s_Column, s_Row);

        if (++s_Column == VGA_WIDTH) 
        {
            s_Column = 0;
            if (++s_Row == VGA_HEIGHT)
            {
                s_Row = 0;
            }
        }
    }

    void Terminal::UpdateCursor()
    {
        uint16_t pos = s_Row * VGA_WIDTH + s_Column;
    
        hal::HAL::OutB(0x3D4, 0x0F);
        hal::HAL::OutB(0x3D5, (uint8_t) (pos & 0xFF));
        hal::HAL::OutB(0x3D4, 0x0E);
        hal::HAL::OutB(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }

    void Terminal::Write(const char* data, size_t size) 
    {
        for (size_t i = 0; i < size; i++)
        {
            PutChar(data[i]);
        }

        if(s_HardwareCursorUpdatesEnabled)
            UpdateCursor();
    }

    void Terminal::WriteString(const char* data) 
    {
        Write(data, strlen(data));
    }

    void Terminal::ClearScreen(uint8_t colour)
    {
        s_Colour = colour;

        for (size_t y = 0; y < VGA_HEIGHT; y++) 
        {
            for (size_t x = 0; x < VGA_WIDTH; x++) 
            {
                const size_t index = y * VGA_WIDTH + x;
                s_Buffer[index] = VGA::Entry(' ', s_Colour);
            }
        }

        s_Row = 0;
        s_Column = 0;
        UpdateCursor();
        SetHardwareCursorUpdateEnabled(true);
    }

    void Terminal::SetCursorPos(size_t row, size_t col)
    {
        s_Row = row;
        s_Column = col;

        if(s_HardwareCursorUpdatesEnabled)
        {
            UpdateCursor();
        }
    }

    void Terminal::GetCursorPos(size_t& row, size_t& col)
    {
        row = s_Row;
        col = s_Column;
    }

    void Terminal::SetHardwareCursorUpdateEnabled(bool enabled)
    {
        s_HardwareCursorUpdatesEnabled = enabled;
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

    hal::KeyCode Terminal::GetUserKeyCode()
    {
		hal::KeyCode key = hal::KeyCode::Unknown;

		//wait for input
		while(key == hal::KeyCode::Unknown)
			key = hal::Keyboard::Get().GetLastKeyCode();

		//key press is handled by the idt, assume it was handled by this point since we didnt throw an exception.
		hal::Keyboard::Get().DiscardLastKeyCode();
		return key;
    }

    void Terminal::GetUserCommand(char* buf, int n)
    {
		WriteString("\nCommand> ");

		hal::KeyCode key = hal::KeyCode::Unknown;
		bool BufChar;

		int i=0;
		while (i < n) 
		{
			BufChar = true;
			key = GetUserKeyCode();

			if (key == hal::KeyCode::Return)
			{
				//command entered, stop parsing
				break;
			}

			if (key == hal::KeyCode::BackSpace) 
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
				char c = hal::Keyboard::Get().KeyCodeToAscii(key);
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
    kernel::Terminal::Write(data, size);
}