#include <string.h>
#include <stdio.h>
#include <kernel/MemoryManagement/PhysicalMemoryManager.h>
#include <kernel/Terminal.h>

namespace kernel::memory
{
	PhysicalMemoryManager PhysicalMemoryManager::s_Instance;

	PhysicalMemoryManager::PhysicalMemoryManager()
		: m_MemorySize(0)
		, m_UsedMemoryBlocks(0)
		, m_MaxMemoryBlocks(0)
		, m_MemoryMap(nullptr)
	{

	}

	void PhysicalMemoryManager::Initialise(size_t memSize, physical_addr bitmap)
	{
		m_MemorySize = memSize;
		m_MemoryMap = (uint32_t*)bitmap;
		m_MaxMemoryBlocks = (GetMemorySize() * 1024) / s_BlockSize;
		m_UsedMemoryBlocks = m_MaxMemoryBlocks;

		memset(m_MemoryMap, 0xf, GetTotalBlockCount() / s_BlocksPerByte);
	}

	void PhysicalMemoryManager::InitialiseRegion(physical_addr base, size_t size)
	{
		int align = base / s_BlockSize;
		int blocks = size / s_BlockSize;

		for (; blocks >= 0; blocks--) 
		{
			UnSetMemoryMapBit(align++);
			m_UsedMemoryBlocks--;
		}

		SetMemoryMapBit(0);	//first block is always set. This ensures allocs cant be 0
	}

	void PhysicalMemoryManager::DeInitialiseRegion(physical_addr base, size_t size)
	{
		int align = base / s_BlockSize;
		int blocks = size / s_BlockSize;

		for (; blocks >= 0; blocks--) 
		{
			SetMemoryMapBit(align++);
			m_UsedMemoryBlocks++;
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

		physical_addr addr = bit * s_BlockSize;
		m_UsedMemoryBlocks++;

		return (void*)addr;
	}

	void PhysicalMemoryManager::FreeBlock(void* p) 
	{
		physical_addr addr = (physical_addr)p;
		int bit = addr / s_BlockSize;

		UnSetMemoryMapBit(bit);

		m_UsedMemoryBlocks--;
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

		physical_addr addr = frame * s_BlockSize;
		m_UsedMemoryBlocks += size;

		return (void*)addr;
	}

	void PhysicalMemoryManager::FreeBlocks(void* p, size_t size)
	{
		physical_addr addr = (physical_addr)p;
		int frame = addr / s_BlockSize;

		for (uint32_t i = 0; i < size; i++)
		{
			UnSetMemoryMapBit(frame + i);
		}

		m_UsedMemoryBlocks -= size;
	}

	size_t PhysicalMemoryManager::GetMemorySize() 
	{
		return m_MemorySize;
	}

	uint32_t PhysicalMemoryManager::GetTotalBlockCount()
	{
		return m_MaxMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetUsedBlockCount() 
	{
		return m_UsedMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetFreeBlockCount()
	{
		return m_MaxMemoryBlocks - m_UsedMemoryBlocks;
	}

	uint32_t PhysicalMemoryManager::GetBlockSize ()
	{
		return s_BlockSize;
	}

	void PhysicalMemoryManager::SetMemoryMapBit(int bit) 
	{
		m_MemoryMap[bit / 32] |= (1 << (bit % 32));
	}

	void PhysicalMemoryManager::UnSetMemoryMapBit(int bit) 
	{
		m_MemoryMap[bit / 32] &= ~ (1 << (bit % 32));
	}

	bool PhysicalMemoryManager::IsMemoryMapBitSet(int bit)
	{
		return m_MemoryMap[bit / 32] &  (1 << (bit % 32));
	}

	int PhysicalMemoryManager::GetFirstFreeMemoryMapBit()
	{
		for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		{
			if (m_MemoryMap[i] != 0xffffffff)
			{
				for (int j = 0; j < 32; j++) 
				{
					int bit = 1 << j;
					if (!(m_MemoryMap[i] & bit))
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
			if (m_MemoryMap[i] != 0xffffffff)
			{
				for (int j = 0; j < 32; j++)
				{
					int bit = 1 << j;
					if (!(m_MemoryMap[i] & bit))
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

	void PhysicalMemoryManager::SetPagingEnabled(bool b)
	{
		if(b)
		{
			asm volatile(
				"mov eax, cr0\n"
				"or eax, 0x80000000\n"		//set bit 31
				"mov cr0, eax\n"
			);
		}
		else
		{
			asm volatile (
			"	and eax, 0x7FFFFFFF\n"
				"mov cr0, eax"
			);
		}
	}

	bool PhysicalMemoryManager::IsPagingEnabled()
	{
		uint32_t res=0;
		asm volatile (
			"mov	eax, cr0\n"
			"mov	%0, eax\n"
		: "=m"(res));

		return (res & 0x80000000) ? true : false;
	}

	void PhysicalMemoryManager::LoadPDBR(physical_addr addr)
	{
		asm volatile (
			"mov eax, %0\n"
			"mov cr3, eax\n"		// PDBR is cr3 register in i86
		: : "m"(addr));
	}

	physical_addr PhysicalMemoryManager::GetPDBR()
	{
		physical_addr pdbr;
		asm volatile (
			"mov eax, cr3\n"
			"mov %0, eax\n"
		: "=m"(pdbr));
	}
}