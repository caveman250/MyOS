#pragma once

#include <stdint.h>
#include <kernel/FileSystem.h>

namespace kernel
{
    enum class FileAttributes
    {
        ReadOnly = 1,
        Hidden = 2,
        System = 4,
        VolumeLabel =  8,
        SubDirectory = 0x10,
        Archive = 0x20,
        Device = 0x60
    };

    struct Directory
    {
        uint8_t   m_Filename[8];
        uint8_t   m_Ext[3];
        uint8_t   m_Attrib;
        uint8_t   m_Reserved;
        uint8_t   m_TimeCreatedMs;
        uint16_t  m_TimeCreated;
        uint16_t  m_DateCreated;
        uint16_t  m_DateLastAccessed;
        uint16_t  m_FirstClusterHiBytes;
        uint16_t  m_LastModTime;
        uint16_t  m_LastModDate;
        uint16_t  m_FirstCluster;
        uint32_t  m_FileSize;

    }__attribute((packed));

    struct MountInfo 
    {
        uint32_t m_NumSectors;
        uint32_t m_FATOffset;
        uint32_t m_NumRootEntries;
        uint32_t m_RootOffset;
        uint32_t m_RootSize;
        uint32_t m_FATSize;
        uint32_t m_FATEntrySize;
    };



    class FAT12
    {
    public:
        static FAT12& Get() { return s_Instance; }

        File OpenDirectory(const char* directoryName);
        void ReadFileChunk(File* file, unsigned char* buffer, unsigned int length);
        File OpenFile(const char* fileName);
        void Initialise();
        void Mount();
        void CloseFile(File* file);
    private:
        File OpenFileInSubDirectory(File subDirectory, const char* filename);
        void ToFAT12FileName(const char* fileName, char* fName, unsigned int fNameLength);

        static constexpr int s_SectorSize = 512;
        FileSystem m_FATFileSystem;
        MountInfo m_MountInfo;
        uint8_t m_FAT[s_SectorSize*2];

        static FAT12 s_Instance;
    };
}