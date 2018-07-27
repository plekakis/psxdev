#include <system/system.h>
#include <gfx/gfx.h>

#define PLANE_ON_OV (0)
#define FULL_GEOM (1)

uint32 yaw = 0, pitch = 0, roll = 0;
float elapsed = 0.0f;

void render()
{	
	SVECTOR angZero = { 0,0,0 };
	SVECTOR	cubeRotation = { pitch, yaw, roll };
	SVECTOR	cubeRotation2 = { pitch * 2, yaw * 2, roll * 2 };
	MATRIX model2World, world2Camera;

	CVECTOR cubeColors[8] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 255, 0, 255 },{ 255, 255, 0 },{ 0, 255, 255 },{ 128, 0, 128 },{ 0, 64, 128 } };
	CVECTOR planeColors[4] = { { 128, 128, 128 },{ 255, 0, 0 },{ 0, 255, 0 },{ 0,0,255 } };

	POINT_SPRITE pointSprites[1];
	setVector(&pointSprites[0].p, 0, 0, 0);
	setColor(&pointSprites[0].c, 255, 127, 0);
	
	pointSprites[0].width = 8;
	pointSprites[0].height = 8;

	Gfx_SetRenderState(RS_PERSP);

	Gfx_BeginSubmission(OT_LAYER_BG);
	{
#if FULL_GEOM
		// Update and set the camera matrix
		{
			const SVECTOR camRotation = { 0, 0, 0 };
			const VECTOR camPosition = { 0, 0, -600 };

			RotMatrix(&camRotation, &world2Camera);
			TransMatrix(&world2Camera, &camPosition);
			Gfx_SetCameraMatrix(&world2Camera);
		}

		// First cube
		{
			const VECTOR	v = { -128, 0, 256 };
			RotMatrix(&cubeRotation, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_F3, 64, cubeColors);
		}

		// Second cube
		{
			const VECTOR	v = { 128, 0, 256 };
			RotMatrix(&cubeRotation2, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_G3, 64, cubeColors);
		}
#endif // FULL_GEOM
		// Point sprites
		{
			const VECTOR	v = { 0, 0, 256 };
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
			const VECTOR	v  = {0, 100, 256};
			RotMatrix(&angZero, &model2World);
			TransMatrix(&model2World, &v);
					
			Gfx_SetModelMatrix(&model2World);
			Gfx_AddPlane(PRIM_TYPE_POLY_G3, 1024, 1024, &planeColors);
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
