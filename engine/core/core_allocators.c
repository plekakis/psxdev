#include "core_allocators.h"
#include "../util/util.h"

///////////////////////////////////////////////////
// SCRATCH ALLOCATOR
///////////////////////////////////////////////////

///////////////////////////////////////////////////
int16 Core_InitScratch(ScratchBuffer* o_buffer, uint32 i_scratchSizeInBytes, uint8 i_alignment)
{
	o_buffer->m_start = (uint8*)malloc3(i_scratchSizeInBytes + i_alignment);
	if (!o_buffer->m_start)
		return E_OUT_OF_MEMORY;

	{
		uint8* alignedPtr = Util_AlignPtr(o_buffer->m_start, i_alignment);
		o_buffer->m_alignment = alignedPtr - o_buffer->m_start;
		o_buffer->m_start = alignedPtr;

		o_buffer->m_end = ((uint8*)o_buffer->m_start) + i_scratchSizeInBytes;
		o_buffer->m_next = o_buffer->m_start;

		memset(o_buffer->m_start, 0xbabababa, i_scratchSizeInBytes);
	}
	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_FreeScratch(ScratchBuffer* i_buffer)
{
	uint8* ptr = (uint8*)i_buffer->m_start - i_buffer->m_alignment;
	free3(ptr);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Core_ResetScratch(ScratchBuffer* i_buffer)
{
	i_buffer->m_next = i_buffer->m_start;
	return E_OK;
}

///////////////////////////////////////////////////
uint32 Core_GetTotalScratch(ScratchBuffer* i_buffer)
{
	return (uint32)i_buffer->m_end - (uint32)i_buffer->m_start;
}

///////////////////////////////////////////////////
uint32 Core_GetFreeScratch(ScratchBuffer* i_buffer)
{
	return (uint32)i_buffer->m_end - (uint32)i_buffer->m_next;
}

///////////////////////////////////////////////////
uint32 Core_GetUsedScratch(ScratchBuffer* i_buffer)
{
	return (uint32)i_buffer->m_next - (uint32)i_buffer->m_start;
}

///////////////////////////////////////////////////
void* Core_AllocScratch(ScratchBuffer* i_buffer, uint32 i_bytes, uint8 i_alignment)
{
	i_buffer->m_next = Util_AlignPtr(i_buffer->m_next, i_alignment);
	{
		uint8 *mem = i_buffer->m_next;
		uint8* next = mem + i_bytes;

		if (next <= i_buffer->m_end)
		{
			i_buffer->m_next = next;
			return mem;
		}
	}
	return NULL;
}

///////////////////////////////////////////////////
bool Core_IsFromScratch(ScratchBuffer* i_buffer, void* i_address)
{
	return ( ((uint32)i_address > (uint32)i_buffer->m_start) && ((uint32)i_address < (uint32)i_buffer->m_end) );
}

///////////////////////////////////////////////////
// STACK ALLOCATOR
///////////////////////////////////////////////////