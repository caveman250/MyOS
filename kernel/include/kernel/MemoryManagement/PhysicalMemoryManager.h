#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel::memory
{
    typedef	uint32_t physical_addr;
    class PhysicalMemoryManager
    {
    public:
        static void Initialise(size_t, physical_addr);
        static void InitialiseRegion(physical_addr, size_t);
        static void	DeInitialiseRegion(physical_addr base, size_t);

        static void* AllocateBlock();
        static void FreeBlock(void*);
        static void* AllocateBlocks(size_t);
        static void FreeBlocks(void*, size_t);

        static size_t GetMemorySize();

        static uint32_t GetUsedBlockCount();
        static uint32_t GetFreeBlockCount();
        static uint32_t GetTotalBlockCount();
        static uint32_t GetBlockSize();

        static void	SetPagingEnabled(bool);
        static bool	IsPagingEnabled();
        static void	LoadPDBR(physical_addr);
        static physical_addr GetPDBR();

    private:
        static inline void SetMemoryMapBit(int bit);
        static inline void UnSetMemoryMapBit(int bit);
        static inline bool IsMemoryMapBitSet(int bit);

        static int GetFirstFreeMemoryMapBit();
        static int GetFirstFreeMemoryMapAreaOfSize(size_t size);
    };
}
