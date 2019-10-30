#include "game.h"
#include "states.h"

#include <hl/caches/caches.h>

ResModel2 *g_model2, *g_model2_1;
MATRIX g_model2World, g_world2Camera;

SVECTOR g_rotation = { 0, 0, 0 };
VECTOR g_position = { 0, 0, 1150 };

SVECTOR g_cameraRotation = { 0, 0, 0 };
VECTOR g_cameraPosition = { 0, ONE/16, -ONE/2 };

StringId g_modelName;
StringId g_textureName;

HL_TextureCache g_textureCache;
HL_ModelCache g_modelCache;

ResTexture* g_texture;

///////////////////////////////////////////////////
void Load_FadeOutCallback()
{
	State_Transition(STATE_LOAD, STATE_MENU);
}

///////////////////////////////////////////////////
void State_Load_Enter()
{
	g_modelName = ID("ROOT\\PSM\\CRATE0.PSM");
	g_textureName = ID("ROOT\\TIM\\BOX4D.TIM");

	Fade_SetFadeOutCallback(Load_FadeOutCallback);

	HL_NewTextureCache(&g_textureCache, 10);
	HL_NewModelCache(&g_modelCache, 4);

	HL_LoadModel(&g_modelCache, g_modelName, &g_model2);
	HL_LoadModel(&g_modelCache, g_modelName, &g_model2_1);
	
	HL_LoadTexture(&g_textureCache, g_textureName, &g_texture);
}

///////////////////////////////////////////////////
void State_Load_Leave()
{
	HL_FreeModelCache(&g_modelCache);
	HL_FreeTextureCache(&g_textureCache);
}

///////////////////////////////////////////////////
void State_Load_Update()
{
	g_rotation.vy += 4;
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
		DivisionParams params;
		uint32 i;
		memset(&params, 0, sizeof(params));

		Gfx_SetRenderState(RS_TEXTURING | RS_DIVISION);
		//Gfx_SetRenderState(RS_MUL_BASECOL);

		//params.m_distances[DIVMODE_32x32] = 10;
		//params.m_distances[DIVMODE_16x16] = 20;
		//params.m_distances[DIVMODE_8x8] = 30;
		//params.m_distances[DIVMODE_4x4] = 40;
		params.m_distances[DIVMODE_2x2] = 50;
		Gfx_SetDivisionParams(&params);

		for (i = 0; i < i_model->m_submeshCount; ++i)
		{
			ResMaterial* material = Res_GetMaterialLink(g_modelName, i);
			Gfx_SetPolyBaseColor(material->m_red, material->m_green, material->m_blue);
			
			Gfx_SetTextureDirect(g_texture->m_tpage, g_texture->m_clut);
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

	DrawPSM(g_model2, 0);
	//DrawPSM(g_model2_1, 750);

	Gfx_EndSubmission();
}