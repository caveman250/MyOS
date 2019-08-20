#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel::memory
{
    typedef	uint32_t physical_addr;
    class PhysicalMemoryManager
    {
    public:
        static PhysicalMemoryManager& Get() { return s_Instance; }

        PhysicalMemoryManager();

        void Initialise(size_t, physical_addr);
        void InitialiseRegion(physical_addr, size_t);
        void DeInitialiseRegion(physical_addr base, size_t);

        void* AllocateBlock();
        void FreeBlock(void*);
        void* AllocateBlocks(size_t);
        void FreeBlocks(void*, size_t);

        size_t GetMemorySize();

        uint32_t GetUsedBlockCount();
        uint32_t GetFreeBlockCount();
        uint32_t GetTotalBlockCount();
        uint32_t GetBlockSize();

        void	SetPagingEnabled(bool);
        bool	IsPagingEnabled();
        void	LoadPDBR(physical_addr);
        physical_addr GetPDBR();

    private:
        int GetFirstFreeMemoryMapBit();
        int GetFirstFreeMemoryMapAreaOfSize(size_t size);

        inline void SetMemoryMapBit(int bit);
        inline void UnSetMemoryMapBit(int bit);
        inline bool IsMemoryMapBitSet(int bit);

       	static constexpr uint32_t s_BlocksPerByte = 8;
        static constexpr uint32_t s_BlockSize = 4096;

        uint32_t m_MemorySize;
        uint32_t m_UsedMemoryBlocks;
        uint32_t m_MaxMemoryBlocks;
        uint32_t* m_MemoryMap;

        static PhysicalMemoryManager s_Instance;
    };
}
