#include "game.h"
#include "states.h"

typedef struct
{
	DR_MODE m_mode;
	TILE	m_tile;

	uint32  m_pad[4];
}FadeBatch;

FadeDir g_lastFadeDir = FADE_DIR_NONE;
FadeDir g_fadeDir = FADE_DIR_NONE;
uint8	g_fadeVal = 0;

void(*fadeInDoneCallback)() = NULL;
void(*fadeOutDoneCallback)() = NULL;

///////////////////////////////////////////////////
void Fade_ClearCallbacks()
{
	fadeInDoneCallback = NULL;
	fadeOutDoneCallback = NULL;
}

///////////////////////////////////////////////////
void Fade_SetFadeInCallback(void(*fadeDoneCallback))
{
	fadeInDoneCallback = fadeDoneCallback;
}

///////////////////////////////////////////////////
void Fade_SetFadeOutCallback(void(*fadeDoneCallback))
{
	fadeOutDoneCallback = fadeDoneCallback;
}

///////////////////////////////////////////////////
bool Fade_IsFinished(FadeDir i_direction)
{
	if (i_direction == g_lastFadeDir)
	{
		if (g_lastFadeDir == FADE_DIR_IN)
			return g_fadeVal == 0u;
		else if (g_lastFadeDir == FADE_DIR_OUT)
			return g_fadeVal == 255u;
	}
	return FALSE;
}

///////////////////////////////////////////////////
void Fade_Start(FadeDir i_direction)
{
	g_lastFadeDir = i_direction;
	g_fadeDir = i_direction;
	g_fadeVal = (i_direction == FADE_DIR_IN) ? 255u : 0u;
}

///////////////////////////////////////////////////
void Start()
{
	State_Initialize();		
}

///////////////////////////////////////////////////
void Update()
{
	State_Update();

	if (g_stateUpdateFunc)
	{
		g_stateUpdateFunc();
	}
}

///////////////////////////////////////////////////
void PreRender()
{
	if (g_statePreRenderFunc)
	{
		g_statePreRenderFunc();
	}
}

///////////////////////////////////////////////////
void DoFade()
{
	// Progress fade value
	int32 nextFade = g_fadeVal;
	
	if (g_fadeDir != FADE_DIR_NONE)
	{
		nextFade += ((g_fadeDir == FADE_DIR_IN) ? -16u : 16u);
		if (nextFade < 0)
		{
			nextFade = 0;
			g_fadeDir = FADE_DIR_NONE;
			if (fadeInDoneCallback)
				fadeInDoneCallback();
		}
		if (nextFade > 255)
		{
			nextFade = 255;
			g_fadeDir = FADE_DIR_NONE;
			if (fadeOutDoneCallback)
				fadeOutDoneCallback();
		}

		g_fadeVal = (uint8)nextFade;

		Gfx_BeginSubmission(OT_LAYER_FG);
		{
			Batch2D fade;
			Gfx_BeginBatch2D(&fade, sizeof(FadeBatch));
			{
				DVECTOR position = { 0, 0 };
				DVECTOR size = { Gfx_GetDisplayWidth(), Gfx_GetDisplayHeight() };
				CVECTOR color = { g_fadeVal, g_fadeVal, g_fadeVal };

				Gfx_Batch2D_AddMode(&fade, BLEND_RATE_SUB, MODE_FLAG_NONE, NULL, NULL);
				Gfx_Batch2D_AddTile(&fade, &position, &size, &color, PRIM_FLAG_SEMI_TRANS);
			}
			Gfx_EndBatch2D(&fade);
		}
		Gfx_EndSubmission();
	}
}

///////////////////////////////////////////////////
void Render()
{
	if (g_stateRenderFunc)
	{
		g_stateRenderFunc();
	}

	// Fade screen
	DoFade();
}

int main()
{
    SystemInitInfo sysInitInfo;
    Util_MemZero(&sysInitInfo, sizeof(SystemInitInfo));

    sysInitInfo.m_isHighResolution			= TRUE;
	sysInitInfo.m_sysStackSizeInBytes		= 4 * 1024;
	sysInitInfo.m_gfxScratchSizeInBytes		= 128 * 1024;
	sysInitInfo.m_coreStackSizeInBytes		= 256 * 1024;
	sysInitInfo.m_coreScratchSizeInBytes	= 2 * 1024;
	sysInitInfo.m_refreshMode				= REFRESH_30_HZ;
    sysInitInfo.m_tvMode					= (*(char *)0xbfc7ff52 == 'E') ? MODE_PAL : MODE_NTSC;
	
	sysInitInfo.AppStartFncPtr  = &Start;
	sysInitInfo.AppUpdateFncPtr = &Update;
	sysInitInfo.AppPreRenderFncPtr = &PreRender;
	sysInitInfo.AppRenderFncPtr = &Render;

    System_Initialize(&sysInitInfo);

    System_MainLoop();
    System_Shutdown();
    return 0;
}
