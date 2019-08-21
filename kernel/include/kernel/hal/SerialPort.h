namespace kernel::hal
{
    class SerialPort
    {
    public:
        enum class SerialPortAddress
        {
            Com1 = 0x3f8,
            Com2 = 0x2f8,
            Com3 = 0x3e8,
            Com4 = 0x2e8
        };

        SerialPort(SerialPortAddress port);
        void Initialise();
        char Read();
        void Write(char c);

    private:
        bool SerialRecieved();
        bool IsTransmitEmpty();

        SerialPortAddress m_Port;
    };
}