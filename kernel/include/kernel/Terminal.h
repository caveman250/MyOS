#ifndef _KERNEL_TERMINAL_H
#define _KERNEL_TERMINAL_H

#include <stddef.h>

namespace kernel::terminal
{
    void Init(void);
    void PutChar(char c);
    void Write(const char* data, size_t size);
    void WriteString(const char* data);
}

extern "C" void terminal_write(const char* data, size_t size);

#endif