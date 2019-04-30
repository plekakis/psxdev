#include "game.h"
#include "states.h"

ResModel g_model;
MATRIX g_model2World, g_world2Camera;

SVECTOR g_rotation = { ONE/2, 0, 0 };
VECTOR g_position = { 0, 0, 150 };

SVECTOR g_cameraRotation = { 0, 0, 0 };
VECTOR g_cameraPosition = { 0, 0, 0 };

///////////////////////////////////////////////////
void Load_FadeOutCallback()
{
	State_Transition(STATE_LOAD, STATE_MENU);
}

///////////////////////////////////////////////////
void State_Load_Enter()
{
	Fade_SetFadeOutCallback(Load_FadeOutCallback);

	Res_ReadLoadTMD(ID("ROOT\\TMD\\THESHIP.TMD"), PRIM_TYPE_POLY_F3, &g_model);
}

///////////////////////////////////////////////////
void State_Load_Leave()
{
	
}

///////////////////////////////////////////////////
void State_Load_Update()
{
	g_rotation.vy += 10;
}

///////////////////////////////////////////////////
void State_Load_PreRender()
{
	
}

///////////////////////////////////////////////////
void State_Load_Render()
{
	Gfx_SetClearColor(32, 32, 32);
	Gfx_SetBackColor(16, 16, 16);

	Gfx_SetLightColor(0, 255, 255, 255);
	Gfx_SetLightVector(0, 0, ONE, -ONE);
	
	Gfx_SetRenderState(RS_LIGHTING | RS_BACKFACE_CULL);

	Gfx_BeginSubmission(OT_LAYER_BG);

	// Camera
	{
		RotMatrix(&g_cameraRotation, &g_world2Camera);
		TransMatrix(&g_world2Camera, &g_cameraPosition);

		Gfx_SetCameraMatrix(&g_world2Camera);
	}

	// Model
	{
		RotMatrix(&g_rotation, &g_model2World);
		TransMatrix(&g_model2World, &g_position);

		Gfx_SetModelMatrix(&g_model2World);
	}

	// Draw
	{
		Gfx_AddPrims(g_model.m_primType, g_model.m_data, g_model.m_polyCount);
	}

	Gfx_EndSubmission();
}