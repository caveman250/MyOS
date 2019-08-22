#pragma once

#include <stddef.h>
#include <stdint.h>
#include <kernel/hal/drivers/Keyboard.h>
#include <kernel/hal/SerialPort.h>

namespace kernel
{
    class Terminal
    {
    public:
        static Terminal& Get() { return s_Instance; }

        Terminal();

        void Initialise();
        void InitialiseSerialPort();
        void PutChar(char c);
        void Write(const char* data, size_t size);
        void WriteString(const char* data);
        void ClearScreen(uint8_t colour = 0);
        void SetCursorPos(size_t row, size_t col);
        void GetCursorPos(size_t& row, size_t& col);
        void SetHardwareCursorUpdateEnabled(bool enabled);
        void UpdateCursor();

        //run interactive mode.
        void Run();
        void GetUserCommand(char* buf, int n);

    private:
        void PutEntryAt(char c, uint8_t color, size_t x, size_t y_);

        hal::drivers::KeyCode GetUserKeyCode();
        bool RunUserCommand(char* cmd_buf);

        void ScrollIfNecessary();

        void ShowHelpMessage();
        void ShowTestHelpMessage();

        static constexpr size_t s_VGAWidth = 80;
        static constexpr size_t s_VGAHeight = 25;

        //used to output logs to a text file
        hal::SerialPort m_SerialPort;

        uint16_t* m_Buffer;

        size_t m_Row;
        size_t m_Column;
        uint8_t m_Colour;

        bool m_HardwareCursorUpdatesEnabled;

        static Terminal s_Instance;
    };
}

extern "C" void terminal_write(const char* data, size_t size);