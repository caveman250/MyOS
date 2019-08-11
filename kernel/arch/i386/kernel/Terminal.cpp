#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <kernel/hal/HAL.h>
#include <kernel/VGA.h>

namespace kernel
{
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;

    static size_t s_Row;
    static size_t s_Column;
    static uint8_t s_Colour;
    static uint16_t* s_Buffer;
    static bool s_HardwareCursorUpdatesEnabled;

    void Terminal::Init(void) 
    {
        s_Buffer = (uint16_t*) 0xB8000;

        ClearScreen(VGA::CreateColour(VGA::COLOUR_LIGHT_GREY, VGA::COLOUR_BLUE));
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

    void Terminal::UpdateCursor(int x, int y)
    {
        uint16_t pos = y * VGA_WIDTH + x;
    
        HAL::OutB(0x3D4, 0x0F);
        HAL::OutB(0x3D5, (uint8_t) (pos & 0xFF));
        HAL::OutB(0x3D4, 0x0E);
        HAL::OutB(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }

    void Terminal::Write(const char* data, size_t size) 
    {
        for (size_t i = 0; i < size; i++)
        {
            PutChar(data[i]);
        }

        if(s_HardwareCursorUpdatesEnabled)
            UpdateCursor(s_Column, s_Row);
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
        UpdateCursor(s_Column, s_Row);
        SetHardwareCursorUpdateEnabled(true);
    }

    void Terminal::SetCursorPos(size_t row, size_t col)
    {
        s_Row = row;
        s_Column = col;
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
}

extern "C" void terminal_write(const char* data, size_t size)
{
    kernel::Terminal::Write(data, size);
}