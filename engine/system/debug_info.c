#include "debug_info.h"
#include "../gfx/gfx.h"
#include "../input/input.h"

typedef enum
{
	DEBUG_OVERLAY_TYPE_GFX		= 0,
	DEBUG_OVERLAY_TYPE_INPUT	= 1
}DEBUG_OVERLAY_TYPE;

uint8 g_debugOverlayIndex = DEBUG_OVERLAY_TYPE_INPUT;
char dbgText[128];

///////////////////////////////////////////////////
void Debug_DrawGfxOverlay()
{
	uint32 framebufferIndex = Gfx_GetFrameBufferIndex();

	// Scratch allocations
	{
		float scratchTotal = (float)Gfx_GetTotalScratch(framebufferIndex) / 1024.0f;
		float scratchFree = (float)Gfx_GetFreeScratch(framebufferIndex) / 1024.0f;
		float scratchUsed = (float)Gfx_GetUsedScratch(framebufferIndex) / 1024.0f;

		sprintf2(dbgText, "Scratch kb: %.2f (total), %.2f (used), %.2f (free)\n", scratchTotal, scratchUsed, scratchFree);
		FntPrint(dbgText);
	}
}

///////////////////////////////////////////////////
void Debug_DrawInputOverlay()
{
	const uint32 inputMask = Input_GetConnectionMask();
	const uint32 numControllers = CountBits(inputMask);
	uint32 index = 0u;

	if (numControllers > 0)
	{
		for (index = 0; index < numControllers; ++index)
		{
			//sprintf(dbgText, "Controller %d: %s\n", index + 1, Input_GetControllerId(index));
			//FntPrint (dbgText);
		}
	}
	else
	{
		FntPrint("No controllers\n");
	}
}

///////////////////////////////////////////////////
void Debug_DrawOverlay()
{
	switch (g_debugOverlayIndex)
	{
	case DEBUG_OVERLAY_TYPE_GFX:
		Debug_DrawGfxOverlay();
		break;
	case DEBUG_OVERLAY_TYPE_INPUT:
		Debug_DrawInputOverlay();
		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////
void Debug_DrawAll()
{
	Debug_DrawOverlay();	
}