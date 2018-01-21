#include "system.h"
#include "../gfx/gfx.h"
#include "../gfx/gfx_scratch.h"
#include "../core/core.h"

static SystemInitInfo* g_initInfo = NULL;
static uint8 g_systemRunning = 1;

///////////////////////////////////////////////////
int16 System_Initialize(SystemInitInfo* i_info)
{
    int16 errcode = E_OK;

    g_initInfo = i_info;

	// Initialize core
	errcode |= Core_Initialize();

    // Initialize graphics
    errcode |= Gfx_Initialize(i_info->m_isInterlaced,
							  i_info->m_isHighResolution,
                              i_info->m_tvMode
                              );

    PadInit(0);     /* initialize input */

    if (g_initInfo && g_initInfo->AppStartFncPtr)
        g_initInfo->AppStartFncPtr();

    return errcode;
}

///////////////////////////////////////////////////
int16 System_MainLoop()
{
	float cpuMs = 0.0f;
	float cpuMsVsync = 0.0f;
	
	const float vsyncMs = (Gfx_GetTvMode() == MODE_PAL) ? 20.0f : 16.666f;
	const float hsyncDivisor = (Gfx_IsHighResolution() ? 2.0f : 1.0f);
	
	while (g_systemRunning)
    {	
		uint32 timeStart, timeEnd, timeEndVsync, gpuTime;
		char dbgText[32];
		const float res = Gfx_GetDisplayHeight() / hsyncDivisor;

		sprintf2(dbgText, "CPU: %.2f (%.2f)\n%i, %i", cpuMs, cpuMsVsync);
		
		Gfx_BeginFrame(&timeStart);
				
        if (g_initInfo && g_initInfo->AppUpdateFncPtr)
        {
            g_initInfo->AppUpdateFncPtr();
        }
	
		cpuMs = (float)(timeEnd - timeStart) * vsyncMs / res;
		cpuMsVsync = (float)(timeEndVsync - timeStart) * vsyncMs / res;

		FntPrint(dbgText);
		FntFlush(-1);
		
		if (g_initInfo && g_initInfo->AppRenderFncPtr)
		{
			g_initInfo->AppRenderFncPtr();
		}

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

	errcode |= Gfx_Shutdown();
	errcode |= Core_Shutdown();

	PadStop();

    return E_OK;
}
