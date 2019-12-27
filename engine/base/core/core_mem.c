#include "core.h"
#include "../util/util.h"

//
// Allocation settings
//
#define CORE_MEM_STRATEGY_BEST_FIT (0u)		// Finds the smallest existing block that a new memory allocation can use.
#define CORE_MEM_STRATEGY_FIRST_FIT (1u)	// Finds the first existing block that a new memory allocation can use.

#define CORE_MEM_SMALLEST_SPLIT_BLOCK (16)	// how many bytes + sizeof(CoreMemHeader) the to-be-split block must have to be considered for splitting.
#define CORE_MEM_STRATEGY CORE_MEM_STRATEGY_BEST_FIT

#define CORE_MEM_ALLOW_BLOCK_SPLITTING (1u)		// enable block splitting during Malloc
#define CORE_MEM_ALLOW_BLOCK_COALESCING (1u)	// enable block coalescing during Free

typedef struct CoreMemBlock
{
	// not to be accessed directly, bit 0 contains usage.
	uint32 m_size;
	
	// pointer to the next block in the chain.
	struct CoreMemBlock* m_next;

	// the actual user memory pointer.
	uint8* m_mem;
}ALIGN(16) CoreMemBlock;

// Start/end memory ranges
uint8 *g_heapStartPtr = NULL;
uint8 *g_heapEndPtr = NULL;
uint8 *g_heapCurrentPtr = NULL;

CoreMemBlock *g_memStartPtr = NULL;
CoreMemBlock *g_memCurrentPtr = NULL;
uint32 g_memAllocated = 0u;

///////////////////////////////////////////////////
void MarkBlockAsUsed(CoreMemBlock* i_block)
{
	VERIFY_ASSERT(i_block != NULL, "Null block provided!");
	i_block->m_size |= 1u;
}

///////////////////////////////////////////////////
void MarkBlockAsFree(CoreMemBlock* i_block)
{
	VERIFY_ASSERT(i_block != NULL, "Null block provided!");
	i_block->m_size &= ~1u;
}

///////////////////////////////////////////////////
bool IsBlockFree(CoreMemBlock* i_block)
{
	VERIFY_ASSERT(i_block != NULL, "IsBlockFree: Null block provided!");
	return (i_block->m_size & 1) == 0;
}

///////////////////////////////////////////////////
uint32 PackAllocationSize(uint32 i_requestedSize)
{
	return i_requestedSize << 1u;
}

///////////////////////////////////////////////////
uint32 UnpackAllocationSize(uint32 i_requestedSize)
{
	return i_requestedSize >> 1u;
}

///////////////////////////////////////////////////
uint32 GetBlockAllocationSize(CoreMemBlock* i_block)
{
	VERIFY_ASSERT(i_block != NULL, "GetBlockAllocationSize: Null block provided!");
	return UnpackAllocationSize(i_block->m_size);
}

///////////////////////////////////////////////////
uint16 Core_InitHeap(uint32 i_start, uint32 i_end)
{
	g_heapCurrentPtr = g_heapStartPtr = Util_AlignPtrUp( (uint8*)i_start, PTR_SIZE );
	g_heapEndPtr = (uint8*)i_end;

	REPORT("Initialised heap, available memory: %.2f Kb", (float)g_sysHeapSize / 1024.0f);
	return E_OK;
}

///////////////////////////////////////////////////
uint32 GetAllocationSize(uint32 i_requestedSize)
{
	return i_requestedSize + sizeof(CoreMemBlock) - PTR_SIZE;
}

///////////////////////////////////////////////////
CoreMemBlock* RequestNewBlock(uint32 i_requestedSize)
{
	CoreMemBlock* current = (CoreMemBlock*)g_heapCurrentPtr;

	// Calculated the allocation size for this block.
	const uint32 allocSize = GetAllocationSize(i_requestedSize);

	// Out-of-memory check.
	if (g_heapCurrentPtr + allocSize > g_heapEndPtr)
	{
#if ALLOC_VERBOSE
		REPORT("Out-of-memory! Requested a new memory block of %u bytes.", i_requestedSize);
#endif // ALLOC_VERBOSE
		return NULL;
	}

	// increment current and update	
	current->m_size = PackAllocationSize(i_requestedSize);
	current->m_mem = (uint8*)current + sizeof(CoreMemBlock) - PTR_SIZE;
	current->m_next = NULL;

	g_heapCurrentPtr += allocSize;
	return current;
}

///////////////////////////////////////////////////
bool CanCoalesceBlock(CoreMemBlock* i_block)
{
#if CORE_MEM_ALLOW_BLOCK_COALESCING
	VERIFY_ASSERT(i_block != NULL, "CanCoalesceBlock: Null block provided!");
	return i_block->m_next && IsBlockFree(i_block->m_next);
#else
	return FALSE;
#endif //CORE_MEM_ALLOW_BLOCK_COALESCING
}

///////////////////////////////////////////////////
CoreMemBlock* CoalesceBlock(CoreMemBlock* i_block)
{
	// Directional coalescing;
	// Coalesces every possible block to the right of source block.
	CoreMemBlock* current = i_block;
	uint32 sourceBlockSize = GetBlockAllocationSize(i_block);
	const uint32 originalSourceBlockSize = sourceBlockSize;

	while ( (current != NULL) && CanCoalesceBlock(current))
	{
		sourceBlockSize += GetBlockAllocationSize(current->m_next);
		
#if ALLOC_VERBOSE
		REPORT("Core_Free: Coalesced block with size %u bytes (source block size now: %u bytes, original: %u bytes", GetBlockAllocationSize(current->m_next), sourceBlockSize, originalSourceBlockSize);
#endif // ALLOC_VERBOSE

		current = current->m_next;
	}

	// Update block size.
	i_block->m_size = PackAllocationSize(sourceBlockSize);
	// Break the chain and point to the last block found.
	i_block->m_next = current->m_next;
}

///////////////////////////////////////////////////
bool CanSplitBlock(CoreMemBlock* i_block, uint32 i_requestedSize)
{
#if CORE_MEM_ALLOW_BLOCK_SPLITTING
	VERIFY_ASSERT(i_block != NULL, "CanSplitBlock: Null block provided!");
	return (GetBlockAllocationSize(i_block) - i_requestedSize) > (sizeof(CoreMemBlock) + CORE_MEM_SMALLEST_SPLIT_BLOCK);
#else
	return FALSE;
#endif // CORE_MEM_ALLOW_BLOCK_SPLITTING
}

///////////////////////////////////////////////////
CoreMemBlock* SplitBlock(CoreMemBlock* i_block, uint32 i_requestedSize, bool* o_blockSplit)
{
	VERIFY_ASSERT(i_block != NULL, "SplitBlock: Null block provided!");

	*o_blockSplit = FALSE;

	// The block size can be much larger than the requested size;
	// See if we can split this block. By design, block-splitting can happen if the remainder block's size is larger than the size for the header + a minimum size (CORE_MEM_SMALLEST_SPLIT_BLOCK).
	if (CanSplitBlock(i_block, i_requestedSize))
	{
		const uint32 srcBlockSize = GetBlockAllocationSize(i_block);
		const uint32 splitBlockSize = srcBlockSize - i_requestedSize;
		
		// Create the next block N bytes after the specified one. Update its size to be the remainder of the split and point it to the next block that the source block pointed at.
		CoreMemBlock* splitBlock = (CoreMemBlock*)(i_block->m_mem + i_requestedSize);
		splitBlock->m_mem = (uint8*)splitBlock + sizeof(CoreMemBlock) - PTR_SIZE;
		splitBlock->m_size = PackAllocationSize(splitBlockSize);
		splitBlock->m_next = i_block->m_next;

		// Update the source block's size to be the actual requested size in bytes and point it to the newly created split block.
		i_block->m_size = PackAllocationSize(i_requestedSize);
		i_block->m_next = splitBlock;

		*o_blockSplit = TRUE;
#if ALLOC_VERBOSE
		REPORT("Core_Malloc: Split block of size %u bytes into 1) %u bytes and 2) %u bytes", srcBlockSize, i_requestedSize, GetBlockAllocationSize(splitBlock));
#endif // ALLOC_VERBOSE
	}
	return i_block;
}

///////////////////////////////////////////////////
CoreMemBlock* GetBlockHeader(void* i_data)
{
	// Extract the header pointer out of a user pointer.
	return (CoreMemBlock*)((uint8*)i_data + PTR_SIZE - sizeof(CoreMemBlock));
}

///////////////////////////////////////////////////
CoreMemBlock* GetFreeBlock(uint32 i_requestedSize, bool* o_blockSplit)
{
	CoreMemBlock* current = g_memStartPtr;
	CoreMemBlock* bestBlockFound = NULL;

	*o_blockSplit = FALSE;

	// Find the closest matching block.
	while (current != NULL)
	{
		if (IsBlockFree(current) && (GetBlockAllocationSize(current) >= i_requestedSize))
		{
#if ALLOC_VERBOSE
			REPORT("Core_Malloc: Found existing block of size %u bytes.", GetBlockAllocationSize(current));
#endif // ALLOC_VERBOSE

#if CORE_MEM_STRATEGY == CORE_MEM_STRATEGY_BEST_FIT
			if (!bestBlockFound || GetBlockAllocationSize(current) < GetBlockAllocationSize(bestBlockFound))
			{
				bestBlockFound = current;
			}
#elif CORE_MEM_STRATEGY == CORE_MEM_STRATEGY_FIRST_FIT

			bestBlockFound = current;
			break;
#endif // CORE_MEM_STRATEGY
		}
		current = current->m_next;
	}
	return bestBlockFound ? SplitBlock(bestBlockFound, i_requestedSize, o_blockSplit) : bestBlockFound;
}

///////////////////////////////////////////////////
void* Core_Malloc(uint32 i_sizeInBytes)
{
	void* mem = NULL;
	CoreMemBlock* block = NULL;
	uint32 allocSize = 0u;
	bool wasBlockSplit = FALSE;

	if (i_sizeInBytes == 0)
		return NULL;

	// Start allocating memory;
	// Align size to pointer size.
	allocSize = Util_AlignUp(i_sizeInBytes, PTR_SIZE);

#if ALLOC_VERBOSE
	REPORT("Core_Malloc: Requesting allocation of %u bytes", allocSize);
#endif // ALLOC_VERBOSE

	// See if a compatible block has already been allocated. If so, return that directly.
	block = GetFreeBlock(allocSize, &wasBlockSplit);
	if (block == NULL)
	{
		block = RequestNewBlock(allocSize);
		if (block != NULL)
		{
			// Initialise start.
			if (g_memStartPtr == NULL)
			{
				g_memStartPtr = block;
			}

			// Chain block.
			if (g_memCurrentPtr != NULL)
			{
				g_memCurrentPtr->m_next = block;
			}
			g_memCurrentPtr = block;
		}
	}
		
	// Successful allocation
	if (block != NULL)
	{
		MarkBlockAsUsed(block);
		mem = (void*)block->m_mem; // update memory to point to the block's data
		g_memAllocated += allocSize; // increase allocated memory counter

		// Verify that the allocation size is correct
		{
			CoreMemBlock* sourceBlock = GetBlockHeader(mem);
			const uint32 blockAllocSize = GetBlockAllocationSize(sourceBlock);
			const uint32 condition = wasBlockSplit ? (blockAllocSize == allocSize) : (blockAllocSize >= allocSize); // if the block was split, we expect the size to match exactly.

			VERIFY_ASSERT(condition, "Allocation size does not match what was requested! (header reports: %u bytes, requested: %u)", blockAllocSize, allocSize);
		}


#if ALLOC_VERBOSE
		REPORT("Core_Malloc: Allocated %u bytes. Free memory now: %u bytes", allocSize, Core_GetFreeMemory());
#endif // ALLOC_VERBOSE
	}
		
	return mem;
}

///////////////////////////////////////////////////
void* Core_CAlloc(uint32 i_elementCount, uint32 i_sizePerElement)
{
	const uint32 sizeInBytes = i_elementCount * i_sizePerElement;
	void* mem = Core_Malloc(sizeInBytes);
	if (mem)
	{
		memset(mem, 0, sizeInBytes);
		return mem;
	}
	return NULL;
}

///////////////////////////////////////////////////
void* Core_Realloc(void* i_address, uint32 i_sizeInBytes)
{
	if (i_address == NULL)
	{
		return Core_Malloc(i_sizeInBytes);
	}
	else
	{
		// Allocate a new block of memory if needed
		CoreMemBlock* block = GetBlockHeader(i_address);
		const uint32 srcAllocSize = GetBlockAllocationSize(block);
		if (i_sizeInBytes != srcAllocSize)
		{
			// If there is a valid allocation size, do the realloc logic.
			if (i_sizeInBytes > 0)
			{
				void* newPtr = Core_Malloc(i_sizeInBytes);
				
				// Copy old in new
				memcpy(newPtr, i_address, MIN(srcAllocSize, i_sizeInBytes));

				// Free old
				Core_Free(i_address);
				return newPtr;
			}
			// Otherwise, free the old memory and return NULL.
			else
			{
				Core_Free(i_address);
				return NULL;
			}
		}
		return i_address;
	}
}

///////////////////////////////////////////////////
void Core_Free(void* i_address)
{
	CoreMemBlock *block = NULL;
	uint32 allocSize = 0u;

	if (i_address == NULL)
		return;
	
	VERIFY_ASSERT(((uint8*)i_address >= g_heapStartPtr) && ((uint8*)i_address < g_heapEndPtr), "Core_Free: Trying to free memory not belonging to the system heap!");

#if ALLOC_VERBOSE
	REPORT("Core_Free: Freeing address %x", i_address);
#endif // ALLOC_VERBOSE

	// Get the header from the user address and extract the allocation size.
	block = GetBlockHeader(i_address);
	// if the block has already been freed, early out.
	if (IsBlockFree(block))
	{
#if ALLOC_VERBOSE
	REPORT("Core_Free: Block is already freed");
#endif // ALLOC_VERBOSE
		return;
	}

	allocSize = GetBlockAllocationSize(block);
	VERIFY_ASSERT(allocSize, "Core_Free: Trying to free 0 bytes, this is probably a memory corruption");

	block = CoalesceBlock(block);
	VERIFY_ASSERT(block, "Core_Free: Block was NULL after coalescing!");

	// Mark the block as free.
	MarkBlockAsFree(block);

	// Decrement the allocated memory.
	g_memAllocated -= allocSize;

#if ALLOC_VERBOSE
	REPORT("Core_Free: Freed %u bytes. Free memory now: %u bytes", allocSize, Core_GetFreeMemory());
#endif // ALLOC_VERBOSE
}

///////////////////////////////////////////////////
uint32 Core_GetFreeMemory()
{
	return g_sysHeapSize - g_memAllocated;
}

///////////////////////////////////////////////////
uint32 Core_GetUsedMemory()
{
	return g_memAllocated;
}

///////////////////////////////////////////////////
uint32 Core_GetTotalMemory()
{
	return Core_GetFreeMemory() + Core_GetUsedMemory();
}