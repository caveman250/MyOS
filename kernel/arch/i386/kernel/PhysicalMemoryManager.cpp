#include <string.h>
#include <kernel/PhysicalMemoryManager.h>
#include <kernel/Terminal.h>

namespace kernel
{
	constexpr uint32_t BLOCKS_PER_BYTE = 8;
	constexpr uint32_t BLOCK_SIZE = 4096;
	constexpr uint32_t BLOCK_ALIGN = BLOCK_SIZE;

	uint32_t s_MemorySize = 0;
	uint32_t s_UsedMemoryBlocks = 0;
	uint32_t s_MaxMemoryBlocks = 0;
	uint32_t* s_MemoryMap = 0;

	void PhysicalMemoryManager::Initialise(size_t memSize, physical_addr bitmap)
	{
		s_MemorySize = memSize;
		s_MemoryMap = (uint32_t*) bitmap;
		s_MaxMemoryBlocks = (GetMemorySize() * 1024) / BLOCK_SIZE;
		s_UsedMemoryBlocks = s_MaxMemoryBlocks;

		memset(s_MemoryMap, 0xf, GetTotalBlockCount() / BLOCKS_PER_BYTE);
	}

	void PhysicalMemoryManager::InitialiseRegion(physical_addr base, size_t size)
	{
		int align = base / BLOCK_SIZE;
		int blocks = size / BLOCK_SIZE;

		for (; blocks >= 0; blocks--) 
		{
			UnSetMemoryMapBit(align++);
			s_UsedMemoryBlocks--;
		}

		SetMemoryMapBit(0);	//first block is always set. This ensures allocs cant be 0
	}

	void PhysicalMemoryManager::DeInitialiseRegion(physical_addr base, size_t size)
	{
		int align = base / BLOCK_SIZE;
		int blocks = size / BLOCK_SIZE;

		for (; blocks >= 0; blocks--) 
		{
			SetMemoryMapBit(align++);
			s_UsedMemoryBlocks++;
		}
	}

	void* PhysicalMemoryManager::AllocateBlock()
	{
		if (GetFreeBlockCount() <= 0)
		{
			return 0;
		}

		int bit = GetFirstFreeMemoryMapBit();

		if (bit == -1)
			return 0;

		SetMemoryMapBit(bit);

		physical_addr addr = bit * BLOCK_SIZE;
		s_UsedMemoryBlocks++;

		return (void*)addr;
	}

	void PhysicalMemoryManager::FreeBlock(void* p) 
	{
		physical_addr addr = (physical_addr)p;
		int bit = addr / BLOCK_SIZE;

		UnSetMemoryMapBit(bit);

		s_UsedMemoryBlocks--;
	}

	void* PhysicalMemoryManager::AllocateBlocks(size_t size)
	{
		if (GetFreeBlockCount() <= size)
			return 0;

		int frame = GetFirstFreeMemoryMapAreaOfSize(size);

		if (frame == -1)
			return 0;

		for (uint32_t i = 0; i < size; i++)
		{
			SetMemoryMapBit(frame + i);
		}

		physical_addr addr = frame * BLOCK_SIZE;
		s_UsedMemoryBlocks += size;

		return (void*)addr;
	}

	void PhysicalMemoryManager::FreeBlocks(void* p, size_t size)
	{
		physical_addr addr = (physical_addr)p;
		int frame = addr / BLOCK_SIZE;

		for (uint32_t i = 0; i < size; i++)
		{
			UnSetMemoryMapBit(frame + i);
		}

		s_UsedMemoryBlocks -= size;
	}

	size_t PhysicalMemoryManager::GetMemorySize() 
	{
		return s_MemorySize;
	}

	uint32_t PhysicalMemoryManager::GetTotalBlockCount()
	{
		return s_MaxMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetUsedBlockCount() 
	{
		return s_UsedMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetFreeBlockCount()
	{
		return s_MaxMemoryBlocks - s_UsedMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetBlockSize ()
	{
		return BLOCK_SIZE;
	}

	inline void PhysicalMemoryManager::SetMemoryMapBit(int bit) 
	{
		s_MemoryMap[bit / 32] |= (1 << (bit % 32));
	}

	inline void PhysicalMemoryManager::UnSetMemoryMapBit(int bit) 
	{
		s_MemoryMap[bit / 32] &= ~ (1 << (bit % 32));
	}

	inline bool PhysicalMemoryManager::IsMemoryMapBitSet(int bit)
	{
		return s_MemoryMap[bit / 32] &  (1 << (bit % 32));
	}

	int PhysicalMemoryManager::GetFirstFreeMemoryMapBit()
	{
		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (s_MemoryMap[i] != 0xffffffff)
			{
				for (int j = 0; j < 32; j++) 
				{
					int bit = 1 << j;
					if (!(s_MemoryMap[i] & bit))
					{
						return i * 4 * 8 + j;
					}
				}
			}
		}

		return -1;
	}

	int PhysicalMemoryManager::GetFirstFreeMemoryMapAreaOfSize(size_t size)
	{
		if (size == 0)
			return -1;

		if (size == 1)
			return GetFirstFreeMemoryMapBit();

		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (s_MemoryMap[i] != 0xffffffff)
			{
				for (int j = 0; j < 32; j++)
				{
					int bit = 1 << j;
					if (!(s_MemoryMap[i] & bit))
					{
						int startingBit = i * 32;
						startingBit += bit; //get the free bit in the dword at index i

						uint32_t free = 0; //loop through each bit to see if its enough space
						for (uint32_t count = 0; count <= size;count++)
						{
							if(!IsMemoryMapBitSet(startingBit+count))
							{
								free++;
							}

							if (free == size)
							{
								return i * 4 * 8 + j;
							}
						}
					}
				}
			}
		}
		return -1;
	}
}