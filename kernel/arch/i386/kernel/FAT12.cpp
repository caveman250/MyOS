#include <kernel/FAT12.h>
#include <string.h>
#include <kernel/hal/drivers/FloppyDisk.h>
#include <kernel/BiosParameterBlock.h>
#include <ctype.h>
#include <stdio.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>

namespace kernel
{
    FAT12 FAT12::s_Instance;

    void FAT12::ToFAT12FileName(const char* fileName, char* outName, unsigned int outNameLength) 
    {
        if (outNameLength > 11)
        {
            return;
        }

        if (!outName || !fileName)
        {
            return;
        }

        memset(outName, ' ', outNameLength);

        constexpr int maxFAT12FileNameLength = 8;
        unsigned int i = 0;
        for (i = 0; i < strlen(fileName) - 1 && i < outNameLength; i++)
        {
            if (fileName[i] == '.' || i == maxFAT12FileNameLength)
            {
                break;
            }

            outName[i] = toupper(fileName[i]);
        }

        //file extension
        if (fileName[i] == '.')
        {
            //TODO currently assuming all extensions are 3 characters or less.
            for (int j = 0; j < 3; j++)
            {
                ++i;
                if (fileName[i])
                {
                    outName[8 + j] = toupper(fileName[i]);
                }
            }
        }
    }

    void FAT12::FromFAT12FileName(const char* fat12Name, char* fileName)
    {
        int currFileNameIndex;

        //file name
        for (currFileNameIndex = 0; currFileNameIndex < 8; ++currFileNameIndex)
        {
            if(fat12Name[currFileNameIndex])
            {
                if(fat12Name[currFileNameIndex] == ' ')
                {
                    break;
                }

                fileName[currFileNameIndex] = tolower(fat12Name[currFileNameIndex]);
            }
        }

        //extension
        fileName[currFileNameIndex] = '.';
        for (int i = 8; i < 11; ++i)
        {
            fileName[++currFileNameIndex] = tolower(fat12Name[i]);
        }
    }

    File FAT12::OpenDirectory(const char* directoryName)
    {
        File file;
        unsigned char* buf;
        Directory* directory;

        char fat12FileName[12] = {0};
        ToFAT12FileName(directoryName, fat12FileName, 11);
        fat12FileName[11] = '\0';

        constexpr int sectorsPerDirectory = 14;
        for (int sector = 0; sector < sectorsPerDirectory; ++sector) 
        {
            //Read root dir
            buf = (unsigned char*) hal::drivers::FloppyDisk::Get().ReadSector(m_MountInfo.m_RootOffset + sector);
            directory = (Directory*)buf;

            constexpr int entriesPerSector = 16;
            for (int entry = 0; entry < entriesPerSector; entry++)
            {
                char fileName[12] = {0};
                memcpy(&fileName, directory->m_Filename, 11);
                fileName[11] = '\0';

                if(strlen(fileName) > 0)
                {
                    if (strcmp((const char*)&fat12FileName, (const char*)&fileName) == 0)
                    {
                        //found the file

                        strcpy(file.m_Name, directoryName);
                        file.m_ID = 0;
                        file.m_CurrentCluster = directory->m_FirstCluster;
                        file.m_Length = directory->m_FileSize;
                        file.m_EndOfFile = 0;

                        if (directory->m_Attrib == (uint8_t)FileAttributes::SubDirectory)
                        {
                            file.m_Flags = FileSystemFlags::Directory;
                        }
                        else
                        {
                            file.m_Flags = FileSystemFlags::File;
                        }

                        return file;
                    }
                }

                directory++;
            }
        }

        file.m_Flags = FileSystemFlags::Invalid;
        return file;
    }

    void FAT12::ReadFileChunk(File* file, unsigned char* buffer, unsigned int length)
    {
        if (file)
        {
            unsigned int physicalSector = 32 + (file->m_CurrentCluster - 1);

            unsigned char* sectorData = (unsigned char*)hal::drivers::FloppyDisk::Get().ReadSector(physicalSector);
            memcpy(buffer, sectorData, length);

            //! locate FAT sector
            unsigned int FAT_Offset = file->m_CurrentCluster + (file->m_CurrentCluster / 2); //multiply by 1.5
            unsigned int FAT_Sector = 1 + (FAT_Offset / s_SectorSize);
            unsigned int entryOffset = FAT_Offset % s_SectorSize;

            //! read 1st FAT sector
            sectorData = (unsigned char*)hal::drivers::FloppyDisk::Get().ReadSector(FAT_Sector);
            memcpy(m_FAT, sectorData, length);

            //! read 2nd FAT sector
            sectorData = (unsigned char*)hal::drivers::FloppyDisk::Get().ReadSector(FAT_Sector + 1);
            memcpy(m_FAT + s_SectorSize, sectorData, length);

            //! read entry for next cluster
            uint16_t nextCluster = *(uint16_t*)&m_FAT[entryOffset];

            //reading 16 bits to get a 12 bit value, need to shift appropriatly the read the correct 12 bit value.
            //if the cluster address is an even number mask out the top 4 bits, as they belong to the next cluster.
            //otherwise shift down 4 bits 
            if(file->m_CurrentCluster & 0x0001)
            {
                nextCluster >>= 4;
            }
            else
            {
                nextCluster &= 0x0FFF;
            }

            //last physical cluster?
            if (nextCluster >= 0xff8)
            {
                file->m_EndOfFile = 1;
                return;
            }

            //test for file corruption
            if (nextCluster == 0)
            {
                file->m_EndOfFile = 1;
                return;
            }

            file->m_CurrentCluster = nextCluster;
        }
    }

    void FAT12::CloseFile(File* file)
    {
        if (file)
        {
            file->m_Flags = FileSystemFlags::Invalid;
        }
    }

    File FAT12::OpenFileInSubDirectory(File subDirectory, const char* filename)
    {
        File subDirFile;

        char fat12FileName[12];
        ToFAT12FileName(filename, fat12FileName, 11);
        fat12FileName[11] = '\0';

        while (!subDirectory.m_EndOfFile) 
        {
            unsigned char buf[512];
            ReadFileChunk(&subDirFile, buf, 512);
            Directory* subDirectory = (Directory*)buf;

            constexpr int filenameEntriesPerChunk = 16;
            for (unsigned int i = 0; i < filenameEntriesPerChunk; i++)
            {
                char currEntryName[12];
                memcpy(currEntryName, subDirectory->m_Filename, 11);
                currEntryName[11] = '\0';

                if (strcmp(currEntryName, fat12FileName) == 0)
                {
                    strcpy(subDirFile.m_Name, filename);
                    subDirFile.m_ID = 0;
                    subDirFile.m_CurrentCluster = subDirectory->m_FirstCluster;
                    subDirFile.m_Length = subDirectory->m_FileSize;
                    subDirFile.m_EndOfFile = 0;

                    if (subDirectory->m_Attrib == (uint8_t)FileAttributes::SubDirectory)
                    {
                        subDirFile.m_Flags = FileSystemFlags::Directory;
                    }
                    else
                    {
                        subDirFile.m_Flags = FileSystemFlags::File;
                    }

                    return subDirFile;
                }

                subDirectory++;
            }
        }

        subDirFile.m_Flags = FileSystemFlags::Invalid;
        return subDirFile;
    }

    File FAT12::OpenFile(const char* fileName) 
    {
        File curDirectory;
        char* currPathCharacter = 0;
        bool inRootDir = true;
        char* path = (char*)fileName;

        //jump to any '\' in the path
        currPathCharacter = strchr(path, '\\');
        if (!currPathCharacter)
        {
            //file is in current directory
            curDirectory = OpenDirectory(path);

            if (curDirectory.m_Flags == FileSystemFlags::File)
            {
                return curDirectory;
            }

            //not found
            File file;
            file.m_Flags = FileSystemFlags::Invalid;
            return file;
        }

        currPathCharacter++;
        while (currPathCharacter) 
        {
            char pathname[16];
            int i = 0;
            for (i = 0; i < 16; i++)
            {
                //keep reading untill another \ or line ending.
                if (currPathCharacter[i] == '\\' || currPathCharacter[i] == '\0')
                {
                    break;
                }

                pathname[i] = currPathCharacter[i];
            }

            pathname[i] = '\0';

            if (inRootDir) 
            {
                curDirectory = OpenDirectory(pathname);
                inRootDir = false;
            }
            else
            {
                curDirectory = OpenFileInSubDirectory(curDirectory, pathname);
            }

            if (curDirectory.m_Flags == FileSystemFlags::Invalid)
            {
                break;
            }
            else if (curDirectory.m_Flags == FileSystemFlags::File)
            {
                return curDirectory;
            }

            //Still in a directory, find next '\'
            currPathCharacter = strchr (currPathCharacter + 1, '\\');

            if (currPathCharacter)
            {
                currPathCharacter++;
            }
        }

        File file;
        file.m_Flags = FileSystemFlags::Invalid;
        return file;
    }

    void FAT12::ListFilesInRootDirectory()
    {
        unsigned char* buf;
        Directory* directory;

        constexpr int sectorsPerDirectory = 14;
        for (int sector = 0; sector < sectorsPerDirectory; ++sector) 
        {
            //Read root dir
            buf = (unsigned char*) hal::drivers::FloppyDisk::Get().ReadSector(m_MountInfo.m_RootOffset + sector);
            directory = (Directory*)buf;

            constexpr int entriesPerSector = 16;
            for (int entry = 0; entry < entriesPerSector; entry++)
            {
                char fileName[12] = {0};
                memcpy(&fileName, directory->m_Filename, 11);
                fileName[11] = '\0';

                if(strlen(fileName) > 0 && strcmp(fileName, "WINVBLOCK  ") != 0)
                {
                    char nameForDisplay[12] = {0};
                    FromFAT12FileName(fileName, nameForDisplay);
                    printf("\n%s", nameForDisplay);
                }

                directory++;
            }
        }
    }

    void FAT12::Mount() 
    {
        BootSector* bootsector = (BootSector*)hal::drivers::FloppyDisk::Get().ReadSector(0);

        m_MountInfo.m_NumSectors = bootsector->m_BiosParmataerBlock.m_NumSectors;
        m_MountInfo.m_FATOffset = 1;
        m_MountInfo.m_FATSize = bootsector->m_BiosParmataerBlock.m_SectorsPerFat;
        m_MountInfo.m_FATEntrySize = 8;
        m_MountInfo.m_NumRootEntries = bootsector->m_BiosParmataerBlock.m_NumDirEntries;
        m_MountInfo.m_RootOffset = (bootsector->m_BiosParmataerBlock.m_NumberOfFats * bootsector->m_BiosParmataerBlock.m_SectorsPerFat) + 1;
        m_MountInfo.m_RootSize = ( bootsector->m_BiosParmataerBlock.m_NumDirEntries * 32 ) / bootsector->m_BiosParmataerBlock.m_BytesPerSector;
    }

    void FAT12::Initialise() 
    {
        strcpy(m_FATFileSystem.m_Name, "FAT12");
        m_FATFileSystem.m_Driver = this;

        FileSystemManager::Get().RegisterFileSystem(&m_FATFileSystem, 0);
        Mount();
    }
}