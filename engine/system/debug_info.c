#include "debug_info.h"
#include "../gfx/gfx.h"
#include "../input/input.h"

#if !CONFIG_FINAL

typedef enum
{
	DEBUG_OVERLAY_TYPE_GFX		= 0,
	DEBUG_OVERLAY_TYPE_INPUT	= 1,
	DEBUG_OVERLAY_TYPE_COUNT	= DEBUG_OVERLAY_TYPE_INPUT + 1
}DEBUG_OVERLAY_TYPE;

uint8 g_debugOverlayIndex = DEBUG_OVERLAY_TYPE_GFX;
char dbgText[128];

///////////////////////////////////////////////////
void Debug_DrawGfxOverlay(DebugPanelInfo* i_info)
{
	uint32 framebufferIndex = Gfx_GetFrameBufferIndex();

	// CPU, GPU timings
	{
		sprintf2
		(
			dbgText, "CPU HSync: %u (%u for VSync), GPU: %u\n", 
			i_info->m_timings.m_cpuEndTime - i_info->m_timings.m_cpuStartTime, 
			i_info->m_timings.m_cpuEndTimeVSync - i_info->m_timings.m_cpuStartTime,
			0//i_info->m_timings.m_gpuEndTime - i_info->m_timings.m_gpuStartTime
		);
		FntPrint(dbgText);
	}

	// Scratch allocations
	{
		const float scratchTotal = (float)Gfx_GetTotalScratch(framebufferIndex) / 1024.0f;
		const float scratchFree = (float)Gfx_GetFreeScratch(framebufferIndex) / 1024.0f;
		const float scratchUsed = (float)Gfx_GetUsedScratch(framebufferIndex) / 1024.0f;
		
		sprintf2(dbgText, "Scratch kb: %.2f (total), %.2f (used), %.2f (free)\n\n", scratchTotal, scratchUsed, scratchFree);
		FntPrint(dbgText);
	}

	// Primitive counts
	{
		sprintf2
		(
			dbgText, "PRIMS\nF3: %u (div: %u, lit: %u, fog: %u)\nFT3: %u (div: %u, lit: %u, fog: %u)\nG3: %u (div: %u, lit: %u, fog: %u)\nGT3: %u (div: %u, lit: %u, fog: %u)",
			i_info->m_gfxPrimCounts.m_primF3, i_info->m_gfxPrimCounts.m_primDivF3, i_info->m_gfxPrimCounts.m_primLitF3, i_info->m_gfxPrimCounts.m_primFogF3,
			i_info->m_gfxPrimCounts.m_primFT3, i_info->m_gfxPrimCounts.m_primDivFT3, i_info->m_gfxPrimCounts.m_primLitFT3, i_info->m_gfxPrimCounts.m_primFogFT3,
			i_info->m_gfxPrimCounts.m_primG3, i_info->m_gfxPrimCounts.m_primDivG3, i_info->m_gfxPrimCounts.m_primLitG3, i_info->m_gfxPrimCounts.m_primFogG3,
			i_info->m_gfxPrimCounts.m_primGT3, i_info->m_gfxPrimCounts.m_primDivGT3, i_info->m_gfxPrimCounts.m_primLitGT3, i_info->m_gfxPrimCounts.m_primFogGT3
		);
		FntPrint(dbgText);
	}
}

///////////////////////////////////////////////////
void Debug_DrawInputOverlay(DebugPanelInfo* i_info)
{
	const uint32 inputMask = Input_GetConnectionMask();
	const uint8 numControllers = Util_CountBits32(inputMask);
	uint32 index = 0u;

	if (numControllers > 0)
	{
		for (index = 0; index < numControllers; ++index)
		{
			sprintf(dbgText, "Controller %d: %s\n", index + 1, Input_GetControllerId(index));
			FntPrint (dbgText);
		}
	}
	else
	{
		FntPrint("No controllers\n");
	}
}

///////////////////////////////////////////////////
void Debug_DrawOverlay(DebugPanelInfo* i_info)
{
	switch (g_debugOverlayIndex)
	{
	case DEBUG_OVERLAY_TYPE_GFX:
		Debug_DrawGfxOverlay(i_info);
		break;
	case DEBUG_OVERLAY_TYPE_INPUT:
		Debug_DrawInputOverlay(i_info);
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////
void Debug_DrawAll(DebugPanelInfo* i_info)
{
	// Cycle between them
	const uint32 baseMask = PADL1 | PADR1 | PADL2 | PADR2;

	if (Input_IsClickedEx(0, baseMask | PADLright, baseMask))
	{
		g_debugOverlayIndex = (g_debugOverlayIndex + 1) % DEBUG_OVERLAY_TYPE_COUNT;
	}
	else if (Input_IsClickedEx(0, baseMask | PADLleft, baseMask))
	{
		g_debugOverlayIndex = (g_debugOverlayIndex == 0u) ? (DEBUG_OVERLAY_TYPE_COUNT - 1) : (g_debugOverlayIndex - 1);
	}

	Debug_DrawOverlay(i_info);
}

#endif // !CONFIG_FINAL