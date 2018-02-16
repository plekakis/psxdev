#include "system.h"
#include "../gfx/gfx.h"
#include "../gfx/gfx_scratch.h"
#include "../input/input.h"
#include "../core/core.h"

static SystemInitInfo* g_initInfo = NULL;
static uint8 g_systemRunning = 1;

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
	errcode |= Core_Initialize();

    // Initialize graphics
    errcode |= Gfx_Initialize(i_info->m_isInterlaced,
							  i_info->m_isHighResolution,
                              i_info->m_tvMode
                              );

	// Initialize input
	errcode |= Input_Initialize();
	
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
	float cpuMs = 0.0f;
	float cpuMsVsync = 0.0f;
	
	const float vsyncMs = (Gfx_GetTvMode() == MODE_PAL) ? 20.0f : 16.666f;
	const float hsyncDivisor = (Gfx_IsHighResolution() ? 2.0f : 1.0f);
	
	while (g_systemRunning)
    {	
		uint32 timeStart, timeEnd, timeEndVsync, gpuTime;		
		char dbgText[128];
		const float res = Gfx_GetDisplayHeight() / hsyncDivisor;

		sprintf2(dbgText, "CPU: %.2f (%.2f)\n", cpuMs, cpuMsVsync);
		FntPrint (dbgText);

		Gfx_BeginFrame(&timeStart);
		
        if (g_initInfo && g_initInfo->AppUpdateFncPtr)
        {
            g_initInfo->AppUpdateFncPtr();
        }
	
		cpuMs = (float)(timeEnd - timeStart) * vsyncMs / res;
		cpuMsVsync = (float)(timeEndVsync - timeStart) * vsyncMs / res;

		// Report controllers
		// TODO: Put all this logging in a debug-only view
		{
			const uint32 inputMask = Input_GetConnectionMask();
			const uint32 numControllers = CountBits(inputMask);
			uint32 index = 0u;
			
			if (numControllers > 0)
			{
				for (index=0; index<numControllers; ++index)
				{
					sprintf2(dbgText, "Controller %d: %s\n", index+1, Input_GetControllerId(index));
					FntPrint (dbgText);
				}
			}
			else
			{
				FntPrint("No controllers\n");
			}
		}
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

	errcode |= Input_Shutdown();
	errcode |= Gfx_Shutdown();
	errcode |= Core_Shutdown();

    return E_OK;
}
