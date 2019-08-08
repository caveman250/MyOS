#include <kernel/Terminal.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "Vga.h"
#include <kernel/sys/io.h>

namespace kernel::terminal
{
    static const size_t VGA_WIDTH = 80;
    static const size_t VGA_HEIGHT = 25;

    static size_t terminal_row;
    static size_t terminal_column;
    static uint8_t terminal_color;
    static uint16_t* terminal_buffer;

    void Init(void) 
    {
        terminal_row = 0;
        terminal_column = 0;
        terminal_color = vga::CreateColour(vga::COLOUR_WHITE, vga::COLOUR_CYAN);
        terminal_buffer = (uint16_t*) 0xB8000;

        for (size_t y = 0; y < VGA_HEIGHT; y++) 
        {
            for (size_t x = 0; x < VGA_WIDTH; x++) 
            {
                const size_t index = y * VGA_WIDTH + x;
                terminal_buffer[index] = vga::Entry(' ', terminal_color);
            }
        }
    }

    void PutEntryAt(char c, uint8_t color, size_t x, size_t y) 
    {
        const size_t index = y * VGA_WIDTH + x;
        terminal_buffer[index] = vga::Entry(c, color);
    }

    void PutChar(char c) 
    {
        if(c == '\n')
        {
            terminal_row++;
            terminal_column = 0;
            return;
        }

        PutEntryAt(c, terminal_color, terminal_column, terminal_row);

        if (++terminal_column == VGA_WIDTH) 
        {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT)
            {
                terminal_row = 0;
            }
        }
    }

    void UpdateCursor(int x, int y)
    {
        uint16_t pos = y * VGA_WIDTH + x;
    
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t) (pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    }

    void Write(const char* data, size_t size) 
    {
        for (size_t i = 0; i < size; i++)
        {
            PutChar(data[i]);
        }

        UpdateCursor(terminal_column, terminal_row);
    }

    void WriteString(const char* data) 
    {
        Write(data, strlen(data));
    }
}

extern "C" void terminal_write(const char* data, size_t size)
{
    kernel::terminal::Write(data, size);
}