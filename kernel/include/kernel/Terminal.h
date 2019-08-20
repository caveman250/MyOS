#pragma once

#include <stddef.h>
#include <stdint.h>
#include <kernel/hal/Keyboard.h>

namespace kernel
{
    class Terminal
    {
    public:
        static void Init();
        static void PutChar(char c);
        static void Write(const char* data, size_t size);
        static void WriteString(const char* data);
        static void ClearScreen(uint8_t colour);
        static void SetCursorPos(size_t row, size_t col);
        static void GetCursorPos(size_t& row, size_t& col);
        static void SetHardwareCursorUpdateEnabled(bool enabled);
        static void UpdateCursor();

        //run interactive mode.
        static void Run();
    private:
        static void PutEntryAt(char c, uint8_t color, size_t x, size_t y_);

        static hal::KeyCode GetUserKeyCode();
        static void GetUserCommand(char* buf, int n);
        static bool RunUserCommand(char* cmd_buf);
    };
}

extern "C" void terminal_write(const char* data, size_t size);