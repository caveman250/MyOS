#ifndef _KERNEL_TERMINAL_H
#define _KERNEL_TERMINAL_H

#include <stddef.h>
#include <stdint.h>

namespace kernel
{
    class Terminal
    {
    public:
        static void Init(void);
        static void PutChar(char c);
        static void Write(const char* data, size_t size);
        static void WriteString(const char* data);
        static void ClearScreen(uint8_t colour);
        static void SetCursorPos(size_t row, size_t col);
        static void GetCursorPos(size_t& row, size_t& col);
        static void SetHardwareCursorUpdateEnabled(bool enabled);
    private:
        static void PutEntryAt(char c, uint8_t color, size_t x, size_t y_);
        static void UpdateCursor(int x, int y);
    };
}

extern "C" void terminal_write(const char* data, size_t size);

#endif