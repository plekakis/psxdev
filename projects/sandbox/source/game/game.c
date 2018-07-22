#include "../engine/system/system.h"
#include "../engine/gfx/gfx.h"

uint32 yaw = 0, pitch = 0, roll = 0;
float elapsed = 0.0f;

void render()
{	
	Gfx_SetRenderState(RS_PERSP);

	Gfx_BeginSubmission(OT_LAYER_BG);
	{
		SVECTOR angZero = {0,0,0};
		SVECTOR	cubeRotation  = { pitch, yaw, roll};
		MATRIX model2World, world2Camera;
				
		CVECTOR cubeColors[8] = { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 0, 255}, {255, 255, 0}, {0, 255, 255}, {128, 0, 128}, {0, 64, 128} };
		CVECTOR planeColors[4] = { {128, 128, 128}, {255, 0, 0}, {0, 255, 0}, {0,0,255} };

		elapsed += 0.01f;

		// Update and set the camera matrix
		{
			const SVECTOR camRotation = {0, 0, 0};
			const VECTOR camPosition = { 600 * cos(elapsed - PI), 0, 600 * -sin(elapsed) };
			//const VECTOR camPosition = { 0, 0, 600 };
			
			RotMatrix(&camRotation, &world2Camera);
			TransMatrix(&world2Camera, &camPosition);
			Gfx_SetCameraMatrix(&world2Camera);
		}
		
		// First cube
		{
			const VECTOR	v  = {-128, 0, 256};
			RotMatrix(&cubeRotation, &model2World);
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_F3, 64, cubeColors);
		}

		// Second cube
		{
			const VECTOR	v  = {128, 0, 256};
			TransMatrix(&model2World, &v);

			Gfx_SetModelMatrix(&model2World);
			Gfx_AddCube(PRIM_TYPE_POLY_G3, 64, cubeColors);
		}

		// And the floor
		{
			const VECTOR	v  = {0, 100, 256};

			TransMatrix(&model2World, &v);
			RotMatrix(&angZero, &model2World);
		
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

    sysInitInfo.m_isHighResolution  = FALSE; // doesn't work on psx :( 
	sysInitInfo.m_isInterlaced		= FALSE; // doesn't work on psx :(
    sysInitInfo.m_tvMode            = MODE_NTSC; // <-- TODO: Pick from BIOS
	
	sysInitInfo.AppRenderFncPtr = &render;

    System_Initialize(&sysInitInfo);
    System_MainLoop();
    System_Shutdown();
    return 0;
}
