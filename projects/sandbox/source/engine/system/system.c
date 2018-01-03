#include "system.h"
#include "../gfx/gfx.h"

static SystemInitInfo* g_initInfo = NULL;
static uint8 g_systemRunning = 1;

///////////////////////////////////////////////////
int16 System_Initialize(SystemInitInfo* i_info)
{
    int16 errcode = E_OK;

    g_initInfo = i_info;

    // Initialize graphics
    errcode |= Gfx_Initialize(i_info->m_isHighResolution,
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
    while (g_systemRunning)
    {
        Gfx_BeginFrame();

        FntPrint("Psx Sandbox");
		FntFlush(-1);

        if (g_initInfo && g_initInfo->AppUpdateFncPtr)
        {
            g_initInfo->AppUpdateFncPtr();
        }

        Gfx_EndFrame();
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

	PadStop();

    return E_OK;
}
