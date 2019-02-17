#include "gfx.h"
#include "../util/util.h"
#include "../core/core_allocators.h"

ScratchBuffer g_scratchBuffers[GFX_NUM_BUFFERS];

///////////////////////////////////////////////////
int16 Gfx_InitScratch(uint32 i_gfxScratchSizeInBytes)
{
	uint8 index;
	
	Util_MemZero(g_scratchBuffers, sizeof(g_scratchBuffers));
	for (index=0; index<ARRAY_SIZE(g_scratchBuffers); ++index)
	{
		uint16 err = Core_InitScratch(&g_scratchBuffers[index], i_gfxScratchSizeInBytes, 4);
		if (err != E_OK)
			return err;
	}

	return E_OK;
}

///////////////////////////////////////////////////
uint32 Gfx_GetTotalScratch()
{
	return Core_GetTotalScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()]);
}

///////////////////////////////////////////////////
uint32 Gfx_GetFreeScratch()
{
	return Core_GetFreeScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()]);
}

///////////////////////////////////////////////////
uint32 Gfx_GetUsedScratch()
{
	return Core_GetUsedScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()]);
}

///////////////////////////////////////////////////
int16 Gfx_ResetScratch()
{
	return Core_ResetScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()]);
}

///////////////////////////////////////////////////
int16 Gfx_FreeScratch()
{
	return Core_FreeScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()]);
}

///////////////////////////////////////////////////
void* Gfx_Alloc(uint32 i_bytes, uint32 i_alignment)
{	
	return Core_AllocScratch(&g_scratchBuffers[Gfx_GetFrameBufferIndex()], i_bytes, i_alignment);
}