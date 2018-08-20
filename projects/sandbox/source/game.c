#include <system/system.h>
#include <gfx/gfx.h>

#define PLANE_ON_OV (0)
#define FULL_GEOM (1)

uint32 yaw = 0, pitch = 0, roll = 0;

int elapsed = 0;

int32 xx = 0, yy = 0, zz = -800;
int32 rx = 0, ry = 0, rz = 0;

const uint32 g_speed = 4;

void input()
{
	uint32 pad = Input_GetPad(0);
	if (pad & PADLright)
	{
		ry += g_speed;
	}
	if (pad & PADLleft)
	{
		ry -= g_speed;
	}
	if (pad & PADLup)
	{
		rx += g_speed;
	}
	if (pad & PADLdown)
	{
		rx -= g_speed;
	}

	if (pad & PADRdown)
	{
		zz += g_speed;
	}

	if (pad & PADRleft)
	{
		zz -= g_speed;
	}

	if (pad & PADR1)
	{
		yy += g_speed;
	}

	if (pad & PADL1)
	{
		yy -= g_speed;
	}
}

void render()
{	
	SVECTOR angZero = { 0,0,0 };
	SVECTOR	cubeRotation = { pitch, yaw, roll };
	SVECTOR	cubeRotation2 = { pitch * 2, yaw * 2, roll * 2 };
	MATRIX model2World, world2Camera;

	CVECTOR cubeColors[8] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 255, 0, 255 },{ 255, 255, 0 },{ 0, 255, 255 },{ 128, 0, 128 },{ 0, 64, 128 } };
	CVECTOR planeColors[4] = { { 128, 128, 128 },{ 255, 0, 0 },{ 0, 255, 0 },{ 0,0,255 } };

	POINT_SPRITE pointSprites[2];
	setVector(&pointSprites[0].p, 0, 0, 0);
	setColor(&pointSprites[0].c, 255, 127, 0);
	
	pointSprites[0].width = 32;
	pointSprites[0].height = 32;
	
	input();

	Gfx_SetRenderState(RS_PERSP);
	//Gfx_SetRenderState(RS_FOG);
	//Gfx_SetFogNearFar(500, 1500);
	//Gfx_SetFogColor(128, 128, 128);

	Gfx_BeginSubmission(OT_LAYER_BG);
	
	{
#if FULL_GEOM
		// Update and set the camera matrix
		{
			SVECTOR camRotation = { rx, ry, rz };
			VECTOR camPosition = { xx, yy, zz };
			
			RotMatrix(&camRotation, &world2Camera);
			TransMatrix(&world2Camera, &camPosition);
			
			Gfx_SetCameraMatrix(&world2Camera);
		}

		// First cube
		{
			VECTOR	v = { -128, 0, 256 };
			RotMatrix(&cubeRotation, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_F3, 64, cubeColors);
		}

		// Second cube
		{
			VECTOR	v = { 128, 0, 256 };
			RotMatrix(&cubeRotation2, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_G3, 64, cubeColors);
		}
#endif // FULL_GEOM
		// Point sprites
		{
			VECTOR	v = { 0, 0, 200 };
			RotMatrix(&angZero, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddPointSprites(PRIM_TYPE_POLY_F3, pointSprites, 1);
		}
	}
#if PLANE_ON_OV
	Gfx_EndSubmission();

	Gfx_BeginSubmission(OT_LAYER_OV);
#endif // PLANE_ON_OV
	{
		// And the floor
		{
			VECTOR	v  = {0, 100, 256};
			RotMatrix(&angZero, &model2World);
			TransMatrix(&model2World, &v);
					
			Gfx_SetModelMatrix(&model2World);
			Gfx_AddPlane(PRIM_TYPE_POLY_G3, 1024, 1024, planeColors);
		}
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

    sysInitInfo.m_isHighResolution  = TRUE;
    sysInitInfo.m_tvMode            = MODE_NTSC; // <-- TODO: Pick from BIOS
	
	sysInitInfo.AppRenderFncPtr = &render;

    System_Initialize(&sysInitInfo);
    System_MainLoop();
    System_Shutdown();
    return 0;
}
