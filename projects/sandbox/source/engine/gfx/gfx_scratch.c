#include "gfx.h"

typedef struct
{
	uint8 *m_start;
	uint8 *m_end;
	uint8 *m_next;
}ScratchBuffer;

static ScratchBuffer* g_scratchBuffers = NULL;
static uint8 g_scratchBufferCount = 0;

#define SCRATCH_SIZE (32 * 1024) // 32k of scratch

///////////////////////////////////////////////////
int16 Gfx_InitScratch(uint8 i_frameBufferCount)
{
	uint8	index;
	uint32	scratchSize = SCRATCH_SIZE * i_frameBufferCount;
	g_scratchBufferCount = i_frameBufferCount;

	g_scratchBuffers = (ScratchBuffer*)calloc3(sizeof(ScratchBuffer) * g_scratchBufferCount);
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
	uint8 *mem = AlignPtr(buffer->m_next + i_bytes, i_alignment);

    if (mem <= buffer->m_end)
	{
		buffer->m_next = mem;
		return mem;
	}

	return NULL;
}