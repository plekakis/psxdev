#include "game.h"

#define SCENE_EMPTY (0xffff)
#define SCENE_PRIM_TEST (0)
#define SCENE_BATCH2D_TEST (1)

#ifndef START_SCENE
#define START_SCENE SCENE_EMPTY
#endif // START_SCENE

// Scene selection
#if START_SCENE == SCENE_PRIM_TEST
#include "scene_prim_test.c"
#elif START_SCENE == SCENE_BATCH2D_TEST
#include "scene_batch2d_test.c"
#else
void start() {}
void update() {}
void render() {}
#endif

int main()
{
    SystemInitInfo sysInitInfo;
    Util_MemZero(&sysInitInfo, sizeof(SystemInitInfo));

    sysInitInfo.m_isHighResolution			= TRUE;
	sysInitInfo.m_gfxScratchSizeInBytes		= 128 * 1024;
	sysInitInfo.m_coreStackSizeInBytes		= 2 * 1024;
	sysInitInfo.m_coreScratchSizeInBytes	= 2 * 1024;
	sysInitInfo.m_refreshMode				= REFRESH_30_HZ;
    sysInitInfo.m_tvMode					= (*(char *)0xbfc7ff52 == 'E') ? MODE_PAL : MODE_NTSC;
	
	sysInitInfo.AppStartFncPtr  = &start;
	sysInitInfo.AppUpdateFncPtr = &update;
	sysInitInfo.AppRenderFncPtr = &render;

    System_Initialize(&sysInitInfo);

    System_MainLoop();
    System_Shutdown();
    return 0;
}
