#include "game.h"
#include "states.h"

int32 g_menuIndex = 0;
bool g_inputAllowed = FALSE;

///////////////////////////////////////////////////
void Menu_FadeInCallback()
{
	g_inputAllowed = TRUE;
}

///////////////////////////////////////////////////
void Menu_FadeOutCallback()
{
	State_Transition(STATE_MENU, STATE_GAME);
}

///////////////////////////////////////////////////
void State_Menu_Enter()
{
	Fade_SetFadeInCallback(Menu_FadeInCallback);
	Fade_SetFadeOutCallback(Menu_FadeOutCallback);

	Fade_Start(FADE_DIR_IN);
}

///////////////////////////////////////////////////
void State_Menu_Leave()
{
	
}

///////////////////////////////////////////////////
void State_Menu_Update()
{
	if (g_inputAllowed)
	{
		if (Input_IsClicked(0, PADLdown))
		{
			g_menuIndex++;
		}

		if (Input_IsClicked(0, PADLup))
		{
			g_menuIndex--;
		}

		if (Input_IsClicked(0, PADRdown))
		{			
			if (g_menuIndex == 0)
			{
				g_inputAllowed = FALSE;
				Fade_Start(FADE_DIR_OUT);
			}
		}
	}
	
	if (g_menuIndex < 0)
		g_menuIndex = 1;
	
	if (g_menuIndex > 1)
		g_menuIndex = 0;
}

///////////////////////////////////////////////////
void State_Menu_PreRender()
{

}

///////////////////////////////////////////////////
void State_Menu_Render()
{
	Gfx_SetClearColor(16, 16, 16);

	Gfx_BeginSubmission(OT_LAYER_BG);
	{
		
	}
	Gfx_EndSubmission();
}