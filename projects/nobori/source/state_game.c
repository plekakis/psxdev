#include "game.h"
#include "map.h"
#include "states.h"

typedef struct
{
	SVECTOR		m_rotation;
	VECTOR		m_translation;
	MATRIX		m_worldToCamera;
}Camera;
Camera g_camera;

///////////////////////////////////////////////////
void Game_FadeInCallback()
{

}

///////////////////////////////////////////////////
void Game_FadeOutCallback()
{

}

///////////////////////////////////////////////////
void AdvanceMap()
{

}

///////////////////////////////////////////////////
void State_Game_Enter()
{
	Fade_SetFadeInCallback(Game_FadeInCallback);
	Fade_SetFadeOutCallback(Game_FadeOutCallback);

	Fade_Start(FADE_DIR_IN);

	Util_MemZero(&g_camera, sizeof(g_camera));
}

///////////////////////////////////////////////////
void State_Game_Leave()
{

}

///////////////////////////////////////////////////
void UpdateCamera()
{
	// Make it look down
	g_camera.m_translation.vx = 0;
	g_camera.m_translation.vy = 0;
	g_camera.m_translation.vz = 0;

	RotMatrix(&g_camera.m_rotation, &g_camera.m_worldToCamera);
	TransMatrix(&g_camera.m_worldToCamera, &g_camera.m_translation);

	// Set the updated camera matrix
	Gfx_SetCameraMatrix(&g_camera.m_worldToCamera);
}

///////////////////////////////////////////////////
void Game_HandleInput()
{
	
}

///////////////////////////////////////////////////
void State_Game_Update()
{
	Game_HandleInput();
}

///////////////////////////////////////////////////
void UpdatePlayer()
{
	
}

///////////////////////////////////////////////////
void State_Game_PreRender()
{
	UpdateCamera();
	UpdatePlayer();
}

///////////////////////////////////////////////////
void State_Game_Render()
{
	// Clear color to light blue
	Gfx_SetClearColor(32, 64, 255);

	// Lighting back color to light grey
	Gfx_SetBackColor(64, 64, 64);

	// Enable backface culling
	Gfx_SetRenderState(RS_BACKFACE_CULL);

	// Setup a default light and enable lighting
	Gfx_SetLightColor(0, 255, 255, 255);
	Gfx_SetLightVector(0, ONE / 4, ONE, -ONE / 4);
	Gfx_SetRenderState(RS_LIGHTING);
		
}