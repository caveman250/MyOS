
#pragma once

#include <stdint.h>


namespace kernel
{
    class FAT12;

    enum class FileSystemFlags
    {
        File = 0,
        Directory = 1,
        Invalid = 2
    };

    class File 
    {
    public:
        char m_Name[32];
        FileSystemFlags m_Flags;
        uint32_t m_Length;
        uint32_t m_ID;
        uint32_t m_EndOfFile;
        uint32_t m_Position;
        uint32_t m_CurrentCluster;
        uint32_t m_DeviceID;

    };

    //TODO abstract filesystem driver eg: FAT12* => BaseFileSystem*
    class FileSystem
    {
    public:
        char m_Name[8];
        FAT12* m_Driver;
    };

    class FileSystemManager
    {
    public:
        static FileSystemManager& Get() { return s_Instance; }

        File OpenFile(const char* name);
        void ReadFileChunk(File* file, unsigned char* buffer, unsigned int length);
        void CloseFile(File* file);
        void RegisterFileSystem(FileSystem*, unsigned int deviceID);
        void UnregisterFileSystem(FileSystem*);
        void UnregisterFileSystemByID(unsigned int deviceID);

    private:
        static FileSystemManager s_Instance;

        static constexpr int s_MaxDevices = 26;
        FileSystem* m_FileSystems[s_MaxDevices];
    };
}