#include "game.h"
#include "states.h"
#include <core/obj_cache.h>

ResModel2 *g_model2, *g_model2_1;
MATRIX g_model2World, g_world2Camera;

SVECTOR g_rotation = { ONE/2, 0, 0 };
VECTOR g_position = { 0, 0, 150 };

SVECTOR g_cameraRotation = { 0, 0, 0 };
VECTOR g_cameraPosition = { 0, 0, 0 };

StringId g_modelName;

ObjCache g_textureCache;
ObjCache g_modelCache;

///////////////////////////////////////////////////
void LoadPSM(StringId i_filename, ResModel2** o_model)
{
	bool isAdded;

	*o_model = ObjCache_Insert(&g_modelCache, i_filename, &isAdded);
	if (isAdded)
	{
		Res_ReadLoadPSM(g_modelName, o_model);
		ObjCache_Update(&g_modelCache, i_filename, *o_model);
	}
}

///////////////////////////////////////////////////
void FreePSM(void** io_psm)
{
	Res_FreePSM((ResModel2**)io_psm);
}

///////////////////////////////////////////////////
void FreeTIM(void** io_tim)
{
	Res_FreeTIM((ResTexture**)io_tim);
}

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

	ObjCache_Create(&g_textureCache, 10, FreeTIM);
	ObjCache_Create(&g_modelCache, 4, FreePSM);

	LoadPSM(g_modelName, &g_model2);
	LoadPSM(g_modelName, &g_model2_1);		
}

///////////////////////////////////////////////////
void State_Load_Leave()
{
	ObjCache_Free(&g_modelCache);
	ObjCache_Free(&g_textureCache);
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
void DrawPSM(ResModel2* i_model, int16 offset)
{
	// Model
	{
		int16 prev = g_position.vx;
		g_position.vx += offset;

		RotMatrix(&g_rotation, &g_model2World);
		TransMatrix(&g_model2World, &g_position);

		Gfx_SetModelMatrix(&g_model2World);

		g_position.vx = prev;
	}

	// Draw
	{
		uint32 i;
		Gfx_InvalidateRenderState(RS_BACKFACE_CULL);
		Gfx_SetRenderState(RS_TEXTURING);
		Gfx_SetRenderState(RS_MUL_BASECOL);

		for (i = 0; i < i_model->m_submeshCount; ++i)
		{
			ResMaterial* material = Res_GetMaterialLink(g_modelName, i);
			Gfx_SetPolyBaseColor(material->m_red, material->m_green, material->m_blue);

			Gfx_AddPrims(i_model->m_submeshes[i].m_primType, i_model->m_submeshes[i].m_data, i_model->m_submeshes[i].m_polyCount);
		}

		Gfx_InvalidateRenderState(RS_MUL_BASECOL);
	}
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

	DrawPSM(g_model2, -100);
	DrawPSM(g_model2_1, 100);

	Gfx_EndSubmission();
}