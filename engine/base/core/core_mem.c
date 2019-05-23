#include "core.h"
#include "../util/util.h"

uint32 g_sysUsedRam = 0;

typedef struct
{
	uint32 m_size;
	uint8* m_ptr;
}CoreMemHeader;

///////////////////////////////////////////////////
void* Core_Malloc(uint32 i_sizeInBytes, uint8 i_alignment)
{
	uint32 headerSize = sizeof(CoreMemHeader);
	uint32 size = i_sizeInBytes + i_alignment + headerSize;
	uint8* ptr = (uint8*)malloc3(size);
	uint8* alignedPtr = Util_AlignPtr(ptr, i_alignment);
	CoreMemHeader* header = (CoreMemHeader*)alignedPtr;

	VERIFY_ASSERT(IS_POW2(i_alignment), "Core_Malloc: Alignment not power of two (%u)", i_alignment);
	VERIFY_ASSERT(ptr, "Core_Malloc: Failed allocating %u bytes. Out of memory (%u bytes over)", size, (size - Core_GetFreeMemory()));
	
	header->m_ptr = ptr;
	header->m_size = size;

	g_sysUsedRam += size;

	REPORT("Core_Malloc: Allocated %u bytes, %u byte aligned. Free memory now: %u bytes", size, i_alignment, Core_GetFreeMemory());
	return alignedPtr + headerSize;
}

///////////////////////////////////////////////////
void Core_Free(void* i_address)
{
	{
		VERIFY_ASSERT(i_address, "Core_Free: trying to free NULL pointer");
	}
	{
		CoreMemHeader* header = (CoreMemHeader*)((uint8*)i_address - sizeof(CoreMemHeader));
		uint32 size = header->m_size;

		VERIFY_ASSERT(size > 0, "Core_Free: Trying to free 0 bytes, this is probably a memory corruption");

		free3(header->m_ptr);
		g_sysUsedRam -= size;

		REPORT("Core_Free: Freed %u bytes. Free memory now: %u bytes", size, Core_GetFreeMemory());
	}
}

///////////////////////////////////////////////////
uint32 Core_GetFreeMemory()
{
	return g_sysRamSize - g_sysUsedRam;
}

///////////////////////////////////////////////////
uint32 Core_GetUsedMemory()
{
	return g_sysUsedRam;
}

///////////////////////////////////////////////////
uint32 Core_GetTotalMemory()
{
	return Core_GetFreeMemory() + Core_GetUsedMemory();
}