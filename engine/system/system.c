#include "system.h"
#include "debug_info.h"
#include "../gfx/gfx.h"
#include "../gfx/gfx_scratch.h"
#include "../input/input.h"
#include "../core/core.h"

SystemInitInfo* g_initInfo = NULL;
uint8 g_systemRunning = 1;

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
    Gfx_Initialize(i_info->m_isHighResolution, i_info->m_tvMode);

	// Initialize input
	Input_Initialize();
	
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
	while (g_systemRunning)
    {	
		uint32 timeStart, timeEnd, timeEndVsync, gpuTime;
		
		Gfx_BeginFrame(&timeStart);
		
        if (g_initInfo && g_initInfo->AppUpdateFncPtr)
        {
            g_initInfo->AppUpdateFncPtr();
        }
	
		if (g_initInfo && g_initInfo->AppRenderFncPtr)
		{
			g_initInfo->AppRenderFncPtr();
		}

#if G_DEBUG
		Debug_DrawAll();
#endif // G_DEBUG				

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
