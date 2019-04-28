#include "states.h"
#include "state_load.c"
#include "state_menu.c"
#include "state_game.c"

StateFunc g_stateUpdateFunc = NULL;
StateFunc g_statePreRenderFunc = NULL;
StateFunc g_stateRenderFunc = NULL;

int8 g_state = -1;

///////////////////////////////////////////////////
void State_Initialize()
{
	State_Transition(g_state, STATE_LOAD);
	State_Update();
}

///////////////////////////////////////////////////
void State_Transition(int8 i_stateFrom, int8 i_stateTo)
{
	if (i_stateFrom != i_stateTo)
	{
		// Leave state
		switch (i_stateFrom)
		{
		case STATE_LOAD:
			State_Load_Leave();
			break;

		case STATE_MENU:
			State_Menu_Leave();
			break;

		case STATE_GAME:
			State_Game_Leave();
			break;
		default:break;
		}

		Fade_ClearCallbacks();

		// And transition to state
		switch (i_stateTo)
		{
		case STATE_LOAD:
			State_Load_Enter();
			break;

		case STATE_MENU:
			State_Menu_Enter();
			break;

		case STATE_GAME:
			State_Game_Enter();
			break;
		default:break;
		}
	}

	g_state = i_stateTo;
}

///////////////////////////////////////////////////
void State_Update()
{
	switch (g_state)
	{
	case STATE_LOAD:
		g_stateUpdateFunc = &State_Load_Update;
		g_statePreRenderFunc = &State_Load_PreRender;
		g_stateRenderFunc = &State_Load_Render;
		break;

	case STATE_MENU:
		g_stateUpdateFunc = &State_Menu_Update;
		g_statePreRenderFunc = &State_Menu_PreRender;
		g_stateRenderFunc = &State_Menu_Render;
		break;

	case STATE_GAME:
		g_stateUpdateFunc = &State_Game_Update;
		g_statePreRenderFunc = &State_Game_PreRender;
		g_stateRenderFunc = &State_Game_Render;
		break;
	default:
		break;
	}
}