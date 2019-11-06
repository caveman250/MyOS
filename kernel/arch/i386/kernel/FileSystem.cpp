#include <kernel/FileSystem.h>
#include <string.h>
#include <kernel/FAT12.h>

namespace kernel
{
    FileSystemManager FileSystemManager::s_Instance;

    File FileSystemManager::OpenFile(const char* name) 
    {
        if (name) 
        {
            unsigned char device = 'a';

            char* filename = (char*)name;

            //TODO Typing the file path in all caps breaks this for some reason :( eg: "A:\FILE.TXT"
            if (name[1] == ':')
            {
                device = name[0];
                filename += 3; //strip first 3 characters
            }

            if (m_FileSystems[device - 'a']) 
            {
                File file = m_FileSystems[device - 'a']->m_Driver->OpenFile(filename);
                file.m_DeviceID = device;
                return file;
            }
        }

        File file;
        file.m_Flags = FileSystemFlags::Invalid;
        return file;
    }

    void FileSystemManager::ListFilesInRootDirectory()
    {
        if (m_FileSystems[0]) 
        {
            m_FileSystems[0]->m_Driver->ListFilesInRootDirectory();
        }
    } 

    void FileSystemManager::ReadFileChunk(File* file, unsigned char* buffer, unsigned int length)
    {
        if (file)
        {
            if (m_FileSystems [file->m_DeviceID - 'a'])
            {
                m_FileSystems[file->m_DeviceID - 'a']->m_Driver->ReadFileChunk(file, buffer, length);
            }
        }
    }

    void FileSystemManager::CloseFile(File* file)
    {
        if (file)
        {
            if (m_FileSystems [file->m_DeviceID - 'a'])
            {
                m_FileSystems[file->m_DeviceID - 'a']->m_Driver->CloseFile(file);
            }
        }
    }


    void FileSystemManager::RegisterFileSystem(FileSystem* fsys, unsigned int deviceID) 
    {
        static int i = 0;

        if (i < s_MaxDevices)
        {
            if (fsys) 
            {
                m_FileSystems[deviceID] = fsys;
                i++;
            }
        }
    }

    void FileSystemManager::UnregisterFileSystem (FileSystem* fsys)
    {
        for (int i = 0; i < s_MaxDevices; i++)
        {
            if (m_FileSystems[i] == fsys)
            {
                m_FileSystems[i] = 0;
            }
        }
    }

    void FileSystemManager::UnregisterFileSystemByID (unsigned int deviceID)
    {
        if (deviceID < s_MaxDevices)
        {
            m_FileSystems[deviceID] = 0;
        }
    }
}