#include "debug_info.h"
#include <base/core/core.h>
#include <base/gfx/gfx.h>
#include <base/input/input.h>

#if !CONFIG_FINAL

// Conversion helpers
#define GET_FRACT(x) ((x) & 1023u)
#define GET_INT(x) ((x) >> 10u)

#define Time_100us FP_Q8_24(0.1f)

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
fixed8_24 cpuMs = 0;
fixed8_24 cpuMsVsync = 0;

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

		if (hsyncsPassed >= Time_FromSeconds(Time_100us))
		{
			cpuMs = Time_ToMilliseconds(hsync);
			cpuMsVsync = Time_ToMilliseconds(hsyncAtVsync);

			hsyncsPassed = 0u;
		}

		sprintf
			(
				dbgText, "CPU: %u.%ums (%u.%ums for VSync), FPS: %u (%u VSync)\n\n",
				FP8_24toI32(cpuMs),
				FP8_24toI32Frac(cpuMs, FP_FRAC_2PT),
				FP8_24toI32(cpuMsVsync),
				FP8_24toI32Frac(cpuMsVsync, FP_FRAC_2PT),
				i_info->m_timings.m_framesPerSecond,
				i_info->m_timings.m_framesPerSecondVSync
			);
	}
	
#if CONFIG_DEBUG | CONFIG_RELEASE
	// GFX Scratch allocations
	{
		const uint32 scratchTotalI = GET_INT(Gfx_GetTotalScratch());
		const uint32 scratchTotalF = GET_FRACT(Gfx_GetTotalScratch());
		const uint32 scratchFreeI = GET_INT(Gfx_GetFreeScratch());
		const uint32 scratchFreeF = GET_FRACT(Gfx_GetFreeScratch());
		const uint32 scratchUsedI = GET_INT(Gfx_GetUsedScratch());
		const uint32 scratchUsedF = GET_FRACT(Gfx_GetUsedScratch());
		
		sprintf(dbgText, "%sGfx Scratch kb: %u.%u (total), %u.%u (used), %u.%u (free)\n", dbgText, scratchTotalI, scratchTotalF, scratchUsedI, scratchUsedF, scratchFreeI, scratchFreeF);
	}

	// Core Scratch allocations
	{
		const uint32 scratchTotalI = GET_INT(Core_GetTotalScratch(&g_coreScratchAlloc));
		const uint32 scratchTotalF = GET_FRACT(Core_GetTotalScratch(&g_coreScratchAlloc));
		const uint32 scratchFreeI = GET_INT(Core_GetFreeScratch(&g_coreScratchAlloc));
		const uint32 scratchFreeF = GET_FRACT(Core_GetFreeScratch(&g_coreScratchAlloc));
		const uint32 scratchUsedI = GET_INT(Core_GetUsedScratch(&g_coreScratchAlloc));
		const uint32 scratchUsedF = GET_FRACT(Core_GetUsedScratch(&g_coreScratchAlloc));
		
		sprintf(dbgText, "%sCore Scratch kb: %u.%u (total), %u.%u (used), %u.%u (free)\n", dbgText, scratchTotalI, scratchTotalF, scratchUsedI, scratchUsedF, scratchFreeI, scratchFreeF);
	}

	// Core Stack allocations
	{
		const uint32 stackTotalI = GET_INT(Core_GetTotalStack(&g_coreStackAlloc));
		const uint32 stackTotalF = GET_FRACT(Core_GetTotalStack(&g_coreStackAlloc));
		const uint32 stackFreeI = GET_INT(Core_GetFreeStack(&g_coreStackAlloc));
		const uint32 stackFreeF = GET_FRACT(Core_GetFreeStack(&g_coreStackAlloc));
		const uint32 stackUsedI = GET_INT(Core_GetUsedStack(&g_coreStackAlloc));
		const uint32 stackUsedF = GET_FRACT(Core_GetUsedStack(&g_coreStackAlloc));
		
		sprintf(dbgText, "%sCore stack kb: %u.%u (total), %u.%u (used), %u.%u (free)\n", dbgText, stackTotalI, stackTotalF, stackUsedI, stackUsedF, stackFreeI, stackFreeF);
	}

	// System RAM
	{
		const uint32 ramTotalI = GET_INT(Core_GetTotalMemory());
		const uint32 ramTotalF = GET_FRACT(Core_GetTotalMemory());
		const uint32 ramFreeI = GET_INT(Core_GetFreeMemory());
		const uint32 ramFreeF = GET_FRACT(Core_GetFreeMemory());
		const uint32 ramUsedI = GET_INT(Core_GetUsedMemory());
		const uint32 ramUsedF = GET_FRACT(Core_GetUsedMemory());
		
		sprintf(dbgText, "%sSystem RAM kb: %u.%u (total), %u.%u (used), %u.%u (free)\n\n", dbgText, ramTotalI, ramTotalF, ramUsedI, ramUsedF, ramFreeI, ramFreeF);
	}
	
	// Primitive counts
	{
		sprintf
		(
			dbgText, "%sPRIMS\nF3: %u (div: %u, lit: %u, fog: %u)\nFT3: %u (div: %u, lit: %u, fog: %u)\nG3: %u (div: %u, lit: %u, fog: %u)\nGT3: %u (div: %u, lit: %u, fog: %u)",
			dbgText,
			i_info->m_gfxPrimCounts.m_primF3, i_info->m_gfxPrimCounts.m_primDivF3, i_info->m_gfxPrimCounts.m_primLitF3, i_info->m_gfxPrimCounts.m_primFogF3,
			i_info->m_gfxPrimCounts.m_primFT3, i_info->m_gfxPrimCounts.m_primDivFT3, i_info->m_gfxPrimCounts.m_primLitFT3, i_info->m_gfxPrimCounts.m_primFogFT3,
			i_info->m_gfxPrimCounts.m_primG3, i_info->m_gfxPrimCounts.m_primDivG3, i_info->m_gfxPrimCounts.m_primLitG3, i_info->m_gfxPrimCounts.m_primFogG3,
			i_info->m_gfxPrimCounts.m_primGT3, i_info->m_gfxPrimCounts.m_primDivGT3, i_info->m_gfxPrimCounts.m_primLitGT3, i_info->m_gfxPrimCounts.m_primFogGT3
		);
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