#ifndef GAME_H__
#define GAME_H__

#include <system/system.h>
#include <core/core.h>
#include <gfx/gfx.h>
#include <stream/stream.h>
#include <res/res.h>
#include <util/util.h>

typedef enum
{
	FADE_DIR_NONE,
	FADE_DIR_IN,
	FADE_DIR_OUT
}FadeDir;

// Fading
bool Fade_IsFinished(FadeDir i_direction);
void Fade_Start(FadeDir i_direction);

void Fade_ClearCallbacks();
void Fade_SetFadeInCallback(void(*fadeDoneCallback));
void Fade_SetFadeOutCallback(void(*fadeDoneCallback));

#endif // GAME_H__