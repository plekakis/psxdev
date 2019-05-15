#include "game.h"
#include "states.h"

ResModel2 g_model2;
ResModel g_model;
MATRIX g_model2World, g_world2Camera;

SVECTOR g_rotation = { ONE/2, 0, 0 };
VECTOR g_position = { 0, 0, 150 };

SVECTOR g_cameraRotation = { 0, 0, 0 };
VECTOR g_cameraPosition = { 0, 0, 0 };


StringId g_modelName;

///////////////////////////////////////////////////
void Load_FadeOutCallback()
{
	State_Transition(STATE_LOAD, STATE_MENU);
}

///////////////////////////////////////////////////
void State_Load_Enter()
{
	g_modelName = ID("ROOT\\PSM\\MODEL.PSM");

	Fade_SetFadeOutCallback(Load_FadeOutCallback);

	//Res_ReadLoadTMD(ID("ROOT\\TMD\\THESHIP.TMD"), PRIM_TYPE_POLY_F3, &g_model);
	Res_ReadLoadPSM(g_modelName, &g_model2);
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
		uint32 i;
		Gfx_InvalidateRenderState(RS_BACKFACE_CULL);
		Gfx_SetRenderState(RS_MUL_BASECOL);

		for (i = 0; i < g_model2.m_submeshCount; ++i)
		{
			ResMaterial* material = Res_GetMaterialLink(g_modelName, i);
			Gfx_SetPolyBaseColor(material->m_red, material->m_green, material->m_blue);
			Gfx_AddPrims(g_model2.m_submeshes[i].m_primType, g_model2.m_submeshes[i].m_data, g_model2.m_submeshes[i].m_polyCount);
		}

		Gfx_InvalidateRenderState(RS_MUL_BASECOL);
	}

	Gfx_EndSubmission();
}