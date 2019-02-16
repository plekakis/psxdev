#include "system.h"
#include "debug_info.h"
#include "../gfx/gfx.h"
#include "../gfx/gfx_scratch.h"
#include "../input/input.h"
#include "../core/core.h"
#include "../util//util.h"

SystemInitInfo* g_initInfo = NULL;
bool g_systemRunning = TRUE;

///////////////////////////////////////////////////
void vsync()
{
	Input_Update();
}

///////////////////////////////////////////////////
int16 System_Initialize(SystemInitInfo* i_info)
{
    int16 errcode = E_OK;

    g_initInfo = i_info;

	ResetCallback();

	// Initialize core
	Core_Initialize();

    // Initialize graphics
    Gfx_Initialize(i_info->m_isHighResolution, i_info->m_tvMode, i_info->m_gfxScratchSizeInBytes);

	// Initialize input
	Input_Initialize();
	
	// TODO: There are missing events during vysnc. Do this in the update instead for now (look in System_MainLoop)
	// Register input update to happen at VSync callback
	// Perhaps game logic should be moved here as well
	VSyncCallback(vsync);

    if (g_initInfo && g_initInfo->AppStartFncPtr)
        g_initInfo->AppStartFncPtr();

    return errcode;
}

///////////////////////////////////////////////////
int16 System_MainLoop()
{
	uint16 timeStart=0, timeEnd=0, timeEndVsync=0, gpuTime=0;
	while (g_systemRunning)
    {
		Gfx_BeginFrame(&timeStart);
		
        if (g_initInfo && g_initInfo->AppUpdateFncPtr)
        {
            g_initInfo->AppUpdateFncPtr();
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

			Gfx_Debug_GetPrimCounts(&debugInfo.m_gfxPrimCounts);

			Debug_DrawAll(&debugInfo);
		}
#endif // !CONFIG_FINAL

        Gfx_EndFrame(&timeEnd, &timeEndVsync, &gpuTime);								
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
	Gfx_Shutdown();
	Core_Shutdown();

    return errcode;
}
