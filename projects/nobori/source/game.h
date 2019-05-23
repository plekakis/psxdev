#ifndef GAME_H__
#define GAME_H__

#include <base/system/system.h>
#include <base/core/core.h>
#include <base/gfx/gfx.h>
#include <base/stream/stream.h>
#include <base/res/res.h>
#include <base/util/util.h>
#include <base/core/obj_cache.h>

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