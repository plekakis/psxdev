#include "system.h"
#include "debug_info.h"
#include "../gfx/gfx.h"
#include "../gfx/gfx_scratch.h"
#include "../stream/stream.h"
#include "../input/input.h"
#include "../core/core.h"
#include "../res/res.h"
#include "../util/util.h"

SystemInitInfo* g_initInfo = NULL;
bool g_systemRunning = TRUE;

typedef struct
{
	uint32 m_timeElapsed;
	uint16 m_frameCount;
	uint8  m_framesPerSecond;
}FrameCount;

///////////////////////////////////////////////////
void vsync()
{
	// Reset core scratch allocator
	Core_ResetScratch(CORE_SCRATCHALLOC);

	Stream_Update();
	Res_Update();
	Input_Update();

	if (g_initInfo && g_initInfo->AppUpdateFncPtr)
	{
		g_initInfo->AppUpdateFncPtr();
	}
}

///////////////////////////////////////////////////
int16 System_Initialize(SystemInitInfo* i_info)
{
    int16 errcode = E_OK;

    g_initInfo = i_info;

	ResetCallback();

	// Initialize core
	Core_Initialize(i_info->m_coreStackSizeInBytes, i_info->m_coreScratchSizeInBytes);

    // Initialize graphics
    Gfx_Initialize(i_info->m_isHighResolution, i_info->m_tvMode, i_info->m_refreshMode, i_info->m_gfxScratchSizeInBytes);

	// Initialize stream & cd
	Stream_Initialize();

	// Initialize resource
	Res_Initialize();

	// Initialize input
	Input_Initialize();
	
	// Input update and application update callback is done here
	VSyncCallback(vsync);

	if (g_initInfo && g_initInfo->AppStartFncPtr)
	{
		g_initInfo->AppStartFncPtr();
	}

    return errcode;
}

///////////////////////////////////////////////////
void UpdateFrameCounter(FrameCount* io_frames, uint16 i_timeStart, uint16 i_timeEnd)
{
	// TODO: make a better, reusable timer for this
	// Convert HSYNC counter to seconds
	// A second is after 15625 HSYNCs for NTSC and after 15733 HSYNCs for PAL
	//
	// Update the counter every quarter of a second
	float durationSeconds = (float)(io_frames->m_timeElapsed - i_timeStart) / (float)((Gfx_GetTvMode() == MODE_PAL) ? 15733 : 15625);
	if (durationSeconds > 0.25f)
	{
		io_frames->m_framesPerSecond = io_frames->m_frameCount / durationSeconds;
		io_frames->m_timeElapsed = 0u;
		io_frames->m_frameCount = 0;
	}
	else
	{
		++io_frames->m_frameCount;
		io_frames->m_timeElapsed += i_timeEnd;
	}
}

///////////////////////////////////////////////////
int16 System_MainLoop()
{	
	uint16 timeStart = 0, timeEnd = 0, timeEndVsync = 0, gpuTime = 0;

#if !CONFIG_FINAL
	FrameCount frames, framesVSync;
	Util_MemZero(&frames, sizeof(frames));
	Util_MemZero(&framesVSync, sizeof(framesVSync));
#endif // !CONFIG_FINAL

	while (g_systemRunning)
    {
		Gfx_BeginFrame(&timeStart);
		
		if (g_initInfo && g_initInfo->AppPreRenderFncPtr)
		{
			g_initInfo->AppPreRenderFncPtr();
		}

		if (g_initInfo && g_initInfo->AppRenderFncPtr)
		{
			g_initInfo->AppRenderFncPtr();
		}

#if !CONFIG_FINAL
		{
			DebugPanelInfo debugInfo;
			Util_MemZero(&debugInfo, sizeof(debugInfo));

			debugInfo.m_timings.m_cpuStartTime = timeStart;
			debugInfo.m_timings.m_cpuEndTime = timeEnd;
			debugInfo.m_timings.m_cpuEndTimeVSync = timeEndVsync;
			debugInfo.m_timings.m_gpuStartTime = 0;
			debugInfo.m_timings.m_gpuEndTime = gpuTime;
			debugInfo.m_timings.m_framesPerSecond = frames.m_framesPerSecond;
			debugInfo.m_timings.m_framesPerSecondVSync = framesVSync.m_framesPerSecond;

			Gfx_Debug_GetPrimCounts(&debugInfo.m_gfxPrimCounts);

			Debug_DrawAll(&debugInfo);
		}
#endif // !CONFIG_FINAL

        Gfx_EndFrame(&timeEnd, &timeEndVsync, &gpuTime);

#if !CONFIG_FINAL
		UpdateFrameCounter(&frames, timeStart, timeEnd);
		UpdateFrameCounter(&framesVSync, timeStart, timeEndVsync);
#endif // !CONFIG_FINAL
    }

    return E_OK;
}

///////////////////////////////////////////////////
int16 System_Shutdown()
{
    int16 errcode = E_OK;

    if (g_initInfo && g_initInfo->AppShutdownFncPtr)
    {
        g_initInfo->AppShutdownFncPtr();
    }

	Input_Shutdown();
	Res_Shutdown();
	Stream_Shutdown();
	Gfx_Shutdown();
	Core_Shutdown();

    return errcode;
}
