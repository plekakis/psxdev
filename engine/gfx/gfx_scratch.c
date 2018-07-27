#include "gfx.h"

typedef struct
{
	uint8 *m_start;
	uint8 *m_end;
	uint8 *m_next;
}ScratchBuffer;

static ScratchBuffer* g_scratchBuffers = NULL;
static uint8 g_scratchBufferCount = 0;

#define SCRATCH_SIZE (32 * 1024)

///////////////////////////////////////////////////
int16 Gfx_InitScratch(uint8 i_frameBufferCount)
{
	uint8	index;
	uint32	scratchSize = SCRATCH_SIZE;
	g_scratchBufferCount = i_frameBufferCount;

	g_scratchBuffers = (ScratchBuffer*)malloc3(sizeof(ScratchBuffer) * g_scratchBufferCount);
	if (!g_scratchBuffers)
	{
		return E_OUT_OF_MEMORY;
	}

	for (index=0; index<g_scratchBufferCount; ++index)
	{
		ScratchBuffer* buffer = &g_scratchBuffers[index];

		buffer->m_start = (uint8*)malloc3(scratchSize);
		buffer->m_end = ((uint8*)buffer->m_start) + scratchSize;
		buffer->m_next = buffer->m_start;
	}

	return E_OK;
}

///////////////////////////////////////////////////
uint32 Gfx_GetTotalScratch(uint8 i_frameBufferIndex)
{
	ScratchBuffer* buffer = &g_scratchBuffers[i_frameBufferIndex];
	return (uint32)buffer->m_end - (uint32)buffer->m_start;
}

///////////////////////////////////////////////////
uint32 Gfx_GetFreeScratch(uint8 i_frameBufferIndex)
{
	ScratchBuffer* buffer = &g_scratchBuffers[i_frameBufferIndex];
	return (uint32)buffer->m_end - (uint32)buffer->m_next;
}

///////////////////////////////////////////////////
uint32 Gfx_GetUsedScratch(uint8 i_frameBufferIndex)
{
	ScratchBuffer* buffer = &g_scratchBuffers[i_frameBufferIndex];
	return (uint32)buffer->m_next - (uint32)buffer->m_start;
}

///////////////////////////////////////////////////
int16 Gfx_ResetScratch(uint8 i_frameBufferIndex)
{
	ScratchBuffer* buffer = &g_scratchBuffers[i_frameBufferIndex];
	buffer->m_next = buffer->m_start;
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_FreeScratch(uint8 i_frameBufferIndex)
{
	if (g_scratchBuffers)
		free3(g_scratchBuffers);
	g_scratchBuffers = NULL;

	return E_OK;
}

///////////////////////////////////////////////////
void* Gfx_Alloc(uint32 i_bytes, uint32 i_alignment)
{	
	ScratchBuffer* buffer = &g_scratchBuffers[Gfx_GetFrameBufferIndex()];
	buffer->m_next = AlignPtr(buffer->m_next, i_alignment);

	{
		uint8 *mem = buffer->m_next;
		uint8* next = mem + i_bytes;

		if (next <= buffer->m_end)
		{
			buffer->m_next = next;
			return mem;
		}
	}
	return NULL;
}