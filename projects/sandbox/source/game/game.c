#include "../engine/system/system.h"
#include "../engine/gfx/gfx.h"

void render()
{
	Gfx_BeginSubmission(OT_LAYER_BG);
	{
		SVECTOR	ang  = { 0, 0, 0};
		VECTOR	vec  = {0, 0, 512};
		MATRIX model2World;			
		PRIM_G3 poly[2];

		RotMatrix(&ang, &model2World);
		TransMatrix(&model2World, &vec);

		setVector(&poly[0].v0, -128, 128, 0);
		setVector(&poly[0].v1, -128, -128, 0);
		setVector(&poly[0].v2, 128, -128, 0);
		setColor(&poly[0].c0, 255, 0, 0);
		setColor(&poly[0].c1, 0, 255, 0);
		setColor(&poly[0].c2, 0, 0, 255);

		setVector(&poly[1].v0, -256, 0, 0);
		setVector(&poly[1].v1, -256, -128, 0);
		setVector(&poly[1].v2, -128, -128, 0);
		setColor(&poly[1].c0, 0, 0, 255);
		setColor(&poly[1].c1, 255, 0, 0);
		setColor(&poly[1].c2, 0, 255, 0);

		Gfx_SetModelMatrix(&model2World);
		Gfx_AddPrims(PRIM_TYPE_POLY_G3, poly, 2, PRIM_FLAG_PERSP);
	}
	Gfx_EndSubmission();
}

int main()
{
    SystemInitInfo sysInitInfo;
    memset(&sysInitInfo, 0, sizeof(SystemInitInfo));

    sysInitInfo.m_isHighResolution  = 0;
	sysInitInfo.m_isInterlaced		= 1;
    sysInitInfo.m_tvMode            = MODE_PAL; // <-- TODO: Pick from BIOS

	sysInitInfo.AppRenderFncPtr = &render;

    System_Initialize(&sysInitInfo);
    System_MainLoop();
    System_Shutdown();
    return 0;
}
