#include "debug_info.h"
#include <base/core/core.h>
#include <base/gfx/gfx.h>
#include <base/input/input.h>

#if !CONFIG_FINAL

Batch2D g_debugBatch2D;
typedef struct
{
	DR_TPAGE	m_tpage;
	CHAR2D		m_chars[512];
}DebugPanelBatch2DData;

typedef enum
{
	DEBUG_OVERLAY_TYPE_SYSGFX	= 0,
	DEBUG_OVERLAY_TYPE_INPUT	= 1,
	DEBUG_OVERLAY_TYPE_COUNT	= DEBUG_OVERLAY_TYPE_INPUT + 1
}DEBUG_OVERLAY_TYPE;

uint8 g_debugOverlayIndex = DEBUG_OVERLAY_TYPE_SYSGFX;
char dbgText[1024];
uint64 hsyncsPassed = 0ull;
float cpuMs = 0.0f;
float cpuMsVsync = 0.0f;

///////////////////////////////////////////////////
void Debug_DrawSysGfxOverlay(DebugPanelInfo* i_info)
{
	DVECTOR position = { 10, 10 };
	CVECTOR color = { 255, 255, 255 };

	// CPU, GPU timings
	{
		const uint32 hsync = i_info->m_timings.m_cpuEndTime - i_info->m_timings.m_cpuStartTime;
		const uint32 hsyncAtVsync = i_info->m_timings.m_cpuEndTimeVSync - i_info->m_timings.m_cpuStartTime;

		hsyncsPassed += (uint64)hsync;

		if (hsyncsPassed >= Time_FromSeconds(0.1f))
		{
			cpuMs = Time_ToMilliseconds(hsync);
			cpuMsVsync = Time_ToMilliseconds(hsyncAtVsync);			

			hsyncsPassed = 0u;
		}

		sprintf2
			(
				dbgText, "CPU: %.2fms (%.2fms for VSync), FPS: %u (%u VSync)\n\n",
				cpuMs, 
				cpuMsVsync,
				i_info->m_timings.m_framesPerSecond,
				i_info->m_timings.m_framesPerSecondVSync
			);
	}
	
#if CONFIG_DEBUG | CONFIG_RELEASE
	// GFX Scratch allocations
	{
		char t[64];
		const float scratchTotal = (float)Gfx_GetTotalScratch() / 1024.0f;
		const float scratchFree = (float)Gfx_GetFreeScratch() / 1024.0f;
		const float scratchUsed = (float)Gfx_GetUsedScratch() / 1024.0f;
		
		sprintf2(t, "Gfx Scratch kb: %.2f (total), %.2f (used), %.2f (free)\n", scratchTotal, scratchUsed, scratchFree);
		strcat(dbgText, t);
	}

	// Core Scratch allocations
	{
		char t[64];
		const float scratchTotal = (float)Core_GetTotalScratch(&g_coreScratchAlloc) / 1024.0f;
		const float scratchFree = (float)Core_GetFreeScratch(&g_coreScratchAlloc) / 1024.0f;
		const float scratchUsed = (float)Core_GetUsedScratch(&g_coreScratchAlloc) / 1024.0f;

		sprintf2(t, "Core Scratch kb: %.2f (total), %.2f (used), %.2f (free)\n", scratchTotal, scratchUsed, scratchFree);
		strcat(dbgText, t);
	}

	// Core Stack allocations
	{
		char t[32];
		const float stackTotal = (float)Core_GetTotalStack(&g_coreStackAlloc) / 1024.0f;
		const float stackFree = (float)Core_GetFreeStack(&g_coreStackAlloc) / 1024.0f;
		const float stackUsed = (float)Core_GetUsedStack(&g_coreStackAlloc) / 1024.0f;

		sprintf2(t, "Core stack kb: %.2f (total), %.2f (used), %.2f (free)\n", stackTotal, stackUsed, stackFree);
		strcat(dbgText, t);
	}

	// System RAM
	{
		char t[32];
		const float ramTotal = (float)Core_GetTotalMemory() / 1024.0f;
		const float ramFree = (float)Core_GetFreeMemory() / 1024.0f;
		const float ramUsed = (float)Core_GetUsedMemory() / 1024.0f;

		sprintf2(t, "System RAM kb: %.2f (total), %.2f (used), %.2f (free)\n\n", ramTotal, ramUsed, ramFree);
		strcat(dbgText, t);
	}
	
	// Primitive counts
	{
		char t[128];
		sprintf
		(
			t, "PRIMS\nF3: %u (div: %u, lit: %u, fog: %u)\nFT3: %u (div: %u, lit: %u, fog: %u)\nG3: %u (div: %u, lit: %u, fog: %u)\nGT3: %u (div: %u, lit: %u, fog: %u)",
			i_info->m_gfxPrimCounts.m_primF3, i_info->m_gfxPrimCounts.m_primDivF3, i_info->m_gfxPrimCounts.m_primLitF3, i_info->m_gfxPrimCounts.m_primFogF3,
			i_info->m_gfxPrimCounts.m_primFT3, i_info->m_gfxPrimCounts.m_primDivFT3, i_info->m_gfxPrimCounts.m_primLitFT3, i_info->m_gfxPrimCounts.m_primFogFT3,
			i_info->m_gfxPrimCounts.m_primG3, i_info->m_gfxPrimCounts.m_primDivG3, i_info->m_gfxPrimCounts.m_primLitG3, i_info->m_gfxPrimCounts.m_primFogG3,
			i_info->m_gfxPrimCounts.m_primGT3, i_info->m_gfxPrimCounts.m_primDivGT3, i_info->m_gfxPrimCounts.m_primLitGT3, i_info->m_gfxPrimCounts.m_primFogGT3
		);
		strcat(dbgText, t);
	}
#endif // CONFIG_DEBUG | CONFIG_RELEASE

	Gfx_Batch2D_AddString(&g_debugBatch2D, dbgText, &position, NULL, &color, s_debugfontClut, PRIM_FLAG_NONE);
}

///////////////////////////////////////////////////
void Debug_DrawInputOverlay(DebugPanelInfo* i_info)
{
	const uint32 inputMask = Input_GetConnectionMask();
	const uint8 numControllers = Util_CountBits32(inputMask);
	DVECTOR position = { 10, 10 };
	CVECTOR color = { 255, 255, 255 };
	uint32 index = 0u;

	if (numControllers > 0)
	{
		for (index = 0; index < numControllers; ++index)
		{
			sprintf(dbgText, "Controller %d: %s\n", index + 1, Input_GetControllerId(index));
			
		}
	}
	else
	{
		sprintf(dbgText, "No controllers\n");
	}

	//Gfx_Batch2D_AddString(&g_debugBatch2D, dbgText, &position, &color, s_debugfontClut, PRIM_FLAG_NONE);
}

///////////////////////////////////////////////////
void Debug_DrawOverlay(DebugPanelInfo* i_info)
{
	switch (g_debugOverlayIndex)
	{
	case DEBUG_OVERLAY_TYPE_SYSGFX:
		Debug_DrawSysGfxOverlay(i_info);
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

	Gfx_BeginSubmission(OT_LAYER_OV);
	{
		DVECTOR position = { 10, 10 };
		CVECTOR color = { 255, 255, 255 };
		
		Gfx_BeginBatch2D(&g_debugBatch2D, sizeof(DebugPanelBatch2DData));
		Gfx_Batch2D_AddModeDirect(&g_debugBatch2D, MODE_FLAG_NONE, NULL, s_debugFontTPage);

		Debug_DrawOverlay(i_info);

		Gfx_EndBatch2D(&g_debugBatch2D);
	}
	Gfx_EndSubmission();
}

#endif // !CONFIG_FINAL