#ifndef _KERNEL_TERMINAL_H
#define _KERNEL_TERMINAL_H

#include <stddef.h>
#include <stdint.h>

namespace kernel::terminal
{
    void Init(void);
    void PutChar(char c);
    void Write(const char* data, size_t size);
    void WriteString(const char* data);
    void ClearScreen(uint8_t colour);
    void SetCursorPos(size_t row, size_t col);
    void GetCursorPos(size_t& row, size_t& col);
    void SetHardwareCursorUpdateEnabled(bool enabled);
}

extern "C" void terminal_write(const char* data, size_t size);

#endif