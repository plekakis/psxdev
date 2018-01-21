#include "../engine/system/system.h"
#include "../engine/gfx/gfx.h"

uint32 yaw = 0, pitch = 0, roll = 0;

void render()
{	
	Gfx_SetRenderState(RS_PERSP);
	Gfx_BeginSubmission(OT_LAYER_BG);
	{
		SVECTOR angZero = {0,0,0};
		SVECTOR	ang  = { pitch, yaw, roll};
		VECTOR	vec1  = {-128, 0, 512};
		VECTOR	vec2  = {128, 0, 512};
		VECTOR	vec3  = {0, 100, 256};
		MATRIX model2World;
		CVECTOR cubeColors[8] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 0, 255}, {255, 255, 0}, {0, 255, 255}, {128, 0, 128}, {0, 64, 128} };
		CVECTOR planeColors[4] = { {128, 128, 128}, {255, 0, 0}, {0, 255, 0}, {0,0,255} };

		RotMatrix(&ang, &model2World);
		TransMatrix(&model2World, &vec1);

		//setVector(&poly[0].v0, -128, 128, 0);
		//setVector(&poly[0].v1, -128, -128, 0);
		//setVector(&poly[0].v2, 128, -128, 0);
		//setColor(&poly[0].c0, 255, 0, 0);
		//setColor(&poly[0].c1, 0, 255, 0);
		//setColor(&poly[0].c2, 0, 0, 255);
		//
		//setVector(&poly[1].v0, -256, 0, 0);
		//setVector(&poly[1].v1, -256, -128, 0);
		//setVector(&poly[1].v2, -128, -128, 0);
		//setColor(&poly[1].c0, 0, 0, 255);
		//setColor(&poly[1].c1, 255, 0, 0);
		//setColor(&poly[1].c2, 0, 255, 0);

		Gfx_SetModelMatrix(&model2World);
		Gfx_AddCube(PRIM_TYPE_POLY_F3, 64, cubeColors);

		TransMatrix(&model2World, &vec2);

		Gfx_SetModelMatrix(&model2World);
		Gfx_AddCube(PRIM_TYPE_POLY_G3, 64, cubeColors);

		TransMatrix(&model2World, &vec3);
		RotMatrix(&angZero, &model2World);
		

		Gfx_SetModelMatrix(&model2World);
		Gfx_AddPlane(PRIM_TYPE_POLY_G3, 512, 256, &planeColors);
	}
	Gfx_EndSubmission();

	yaw += 10;
	pitch += 7;
	roll += 5;
}

int main()
{
    SystemInitInfo sysInitInfo;
    memset(&sysInitInfo, 0, sizeof(SystemInitInfo));

    sysInitInfo.m_isHighResolution  = 1;
	sysInitInfo.m_isInterlaced		= 1;
    sysInitInfo.m_tvMode            = MODE_PAL; // <-- TODO: Pick from BIOS

	sysInitInfo.AppRenderFncPtr = &render;

    System_Initialize(&sysInitInfo);
    System_MainLoop();
    System_Shutdown();
    return 0;
}
