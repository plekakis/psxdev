#include "game.h"

#define SCENE_EMPTY (0xffff)
#define SCENE_PRIM_TEST (0)
#define SCENE_BATCH2D_TEST (1)
#define SCENE_CD_TEST (2)
#define SCENE_MODEL (3)
#define SCENE_PARTICLES (4)
#define SCENE_CONTAINERS (5)
#define SCENE_HL_MODEL (6)

#ifndef START_SCENE
#define START_SCENE SCENE_EMPTY
#endif // START_SCENE

// Scene selection
#if START_SCENE == SCENE_PRIM_TEST
#include "scene_prim_test.c"
#elif START_SCENE == SCENE_BATCH2D_TEST
#include "scene_batch2d_test.c"
#elif START_SCENE == SCENE_CD_TEST
#include "scene_cd_test.c"
#elif START_SCENE == SCENE_MODEL
#include "scene_model.c"
#elif START_SCENE == SCENE_PARTICLES
#include "scene_particles.c"
#elif START_SCENE == SCENE_CONTAINERS
#include "scene_containers.c"
#elif START_SCENE == SCENE_HL_MODEL
#include "scene_hl_model.c"
#else
void start() {}
void update() {}
void render() {}
#endif

int main()
{
    SystemInitInfo sysInitInfo;
    Util_MemZero(&sysInitInfo, sizeof(SystemInitInfo));

	sysInitInfo.m_displayWidth				= 640;
	sysInitInfo.m_displayHeight				= 512;
	sysInitInfo.m_sysStackSizeInBytes		= 4 * 1024;
	sysInitInfo.m_gfxScratchSizeInBytes		= 256 * 1024;
	sysInitInfo.m_coreStackSizeInBytes		= 512 * 1024;
	sysInitInfo.m_coreScratchSizeInBytes	= 128 * 1024;
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
