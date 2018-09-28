#include <system/system.h>
#include <gfx/gfx.h>
#include <util/util.h>

#define PLANE_ON_OV (0)
#define FULL_GEOM (1)

uint32 yaw = 0, pitch = 0, roll = 0;

int elapsed = 0;

int32 xx = 0, yy = 0, zz = -800;
int32 rx = 0, ry = 0, rz = 0;

const uint32 g_speed = 4;

CVECTOR cubeColors[8] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 255, 0, 255 },{ 255, 255, 0 },{ 0, 255, 255 },{ 128, 0, 128 },{ 0, 64, 128 } };
CVECTOR planeColors[4] = { { 128, 128, 128 },{ 255, 0, 0 },{ 0, 255, 0 },{ 0,0,255 } };

SVECTOR angZero = { 0,0,0 };
SVECTOR	cubeRotation;
MATRIX model2World, world2Camera;

void input()
{
	if (Input_IsPressed(0, PADLright))
	{
		ry += g_speed;
	}
	if (Input_IsPressed(0, PADLleft))
	{
		ry -= g_speed;
	}
	if (Input_IsPressed(0, PADLup))
	{
		rx += g_speed;
	}
	if (Input_IsPressed(0, PADLdown))
	{
		rx -= g_speed;
	}

	if (Input_IsPressed(0, PADRdown))
	{
		zz += g_speed;
	}

	if (Input_IsPressed(0, PADRleft))
	{
		zz -= g_speed;
	}

	if (Input_IsPressed(0, PADR1))
	{
		yy += g_speed;
	}

	if (Input_IsPressed(0, PADLright))
	{
		yy -= g_speed;
	}
}

void renderCube(PRIM_TYPE type, uint32 x, uint32 y, uint32 z)
{
	VECTOR v = { x, y, z };
	RotMatrix(&cubeRotation, &model2World);
	TransMatrix(&model2World, &v);

	Gfx_SetModelMatrix(&model2World);
	Gfx_AddCube(type, 64, cubeColors);
}

void renderParticles()
{
	VECTOR	v = { 0, 0, 200 };
	POINT_SPRITE pointSprites[2];
	setVector(&pointSprites[0].p, 0, 0, 0);
	setColor(&pointSprites[0].c, 255, 127, 0);

	pointSprites[0].width = 32;
	pointSprites[0].height = 32;
		
	RotMatrix(&angZero, &model2World);
	TransMatrix(&model2World, &v);

	Gfx_SetModelMatrix(&model2World);
	Gfx_AddPointSprites(PRIM_TYPE_POLY_F3, pointSprites, 1);
}

void render()
{	
	Gfx_SetBackColor(16, 16, 16);
	
	Gfx_SetLightColor(0, 255, 255, 255);
	Gfx_SetLightVector(0, -ONE / 2, ONE / 4, ONE);

	// Uncomment to enable a 2nd light pointing to the right, colour dark blue
	//Gfx_SetLightColor(1, 0, 0, 100);
	//Gfx_SetLightVector(1, ONE, 0, 0);

	input();

	// Fog settings
	{
		//Gfx_SetRenderState(RS_FOG);
		Gfx_SetFogNearFar(500, 1500);
		Gfx_SetFogColor(128, 128, 128);
	}	

	Gfx_BeginSubmission(OT_LAYER_BG);
	
	cubeRotation.vx = pitch;
	cubeRotation.vy = yaw;
	cubeRotation.vz = roll;

	{
		// Update and set the camera matrix
		{
			SVECTOR camRotation = { rx, ry, rz };
			VECTOR camPosition = { xx, yy, zz };
			
			RotMatrix(&camRotation, &world2Camera);
			TransMatrix(&world2Camera, &camPosition);
			
			Gfx_SetCameraMatrix(&world2Camera);
		}
				
		// Non-lit group
		Gfx_InvalidateRenderState(RS_LIGHTING);
		{
			renderCube(PRIM_TYPE_POLY_F3 , -512, 0, 256);
			renderCube(PRIM_TYPE_POLY_G3, -256, 0, 256);
		}

		// Lit group
		Gfx_SetRenderState(RS_LIGHTING);
		{
			renderCube(PRIM_TYPE_POLY_F3, 256, 0, 256);
			renderCube(PRIM_TYPE_POLY_G3, 512, 0, 256);
		}

		// Point sprites
		Gfx_InvalidateRenderState(RS_LIGHTING);
		{
			renderParticles();
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

	yaw += 12;
	pitch += 12;
	roll += 12;		
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
