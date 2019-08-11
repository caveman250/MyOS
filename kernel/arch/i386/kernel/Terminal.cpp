#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <kernel/hal/HAL.h>
#include <kernel/Vga.h>

namespace kernel::terminal
{
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;

    static size_t s_Row;
    static size_t s_Column;
    static uint8_t s_Colour;
    static uint16_t* s_Buffer;
    static bool s_HardwareCursorUpdatesEnabled;

    void Init(void) 
    {
        s_Buffer = (uint16_t*) 0xB8000;

        ClearScreen(vga::CreateColour(vga::COLOUR_LIGHT_GREY, vga::COLOUR_BLUE));
        SetHardwareCursorUpdateEnabled(true);
    }

    void PutEntryAt(char c, uint8_t color, size_t x, size_t y) 
    {
        const size_t index = y * VGA_WIDTH + x;
        s_Buffer[index] = vga::Entry(c, color);
    }

    void PutChar(char c) 
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

    void UpdateCursor(int x, int y)
    {
        uint16_t pos = y * VGA_WIDTH + x;
    
        HAL::OutB(0x3D4, 0x0F);
        HAL::OutB(0x3D5, (uint8_t) (pos & 0xFF));
        HAL::OutB(0x3D4, 0x0E);
        HAL::OutB(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }

    void Write(const char* data, size_t size) 
    {
        for (size_t i = 0; i < size; i++)
        {
            PutChar(data[i]);
        }

        if(s_HardwareCursorUpdatesEnabled)
            UpdateCursor(s_Column, s_Row);
    }

    void WriteString(const char* data) 
    {
        Write(data, strlen(data));
    }

    void ClearScreen(uint8_t colour)
    {
        s_Colour = colour;

        for (size_t y = 0; y < VGA_HEIGHT; y++) 
        {
            for (size_t x = 0; x < VGA_WIDTH; x++) 
            {
                const size_t index = y * VGA_WIDTH + x;
                s_Buffer[index] = vga::Entry(' ', s_Colour);
            }
        }

        s_Row = 0;
        s_Column = 0;
        UpdateCursor(s_Column, s_Row);
        SetHardwareCursorUpdateEnabled(true);
    }

    void SetCursorPos(size_t row, size_t col)
    {
        s_Row = row;
        s_Column = col;
    }

    void GetCursorPos(size_t& row, size_t& col)
    {
        row = s_Row;
        col = s_Column;
    }

    void SetHardwareCursorUpdateEnabled(bool enabled)
    {
        s_HardwareCursorUpdatesEnabled = enabled;
    }
}

extern "C" void terminal_write(const char* data, size_t size)
{
    kernel::terminal::Write(data, size);
}