#include <kernel/hal/SerialPort.h>
#include <kernel/hal/HAL.h>

namespace kernel::hal
{
        SerialPort::SerialPort(SerialPortAddress port)
        {
            m_Port = port;
        }

        void SerialPort::Initialise()
        {
            HAL& hal = HAL::Get();

            //TODO make some enums to cover these addresses / masks
            hal.OutB((unsigned short)m_Port + 1, 0x00);    // Disable all interrupts
            hal.OutB((unsigned short)m_Port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
            hal.OutB((unsigned short)m_Port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
            hal.OutB((unsigned short)m_Port + 1, 0x00);    //                  (hi byte)
            hal.OutB((unsigned short)m_Port + 3, 0x03);    // 8 bits, no parity, one stop bit
            hal.OutB((unsigned short)m_Port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
            hal.OutB((unsigned short)m_Port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
        }

        char SerialPort::Read()
        {
            while (!SerialRecieved())
            {}

            return HAL::Get().InB((unsigned short)m_Port);
        }

        void SerialPort::Write(char c)
        {
            while (!IsTransmitEmpty())
            {}
            
            HAL::Get().OutB((unsigned short)m_Port, c);
        }

        bool SerialPort::SerialRecieved()
        {
            return (HAL::Get().InB((unsigned short)m_Port + 5) & 1) != 0;
        }

        bool SerialPort::IsTransmitEmpty()
        {
            return (HAL::Get().InB((unsigned short)m_Port + 5) & 0x20) != 0;
        }
}