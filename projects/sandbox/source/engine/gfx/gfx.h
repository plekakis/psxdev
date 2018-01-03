#ifndef GFX_H_INC
#define GFX_H_INC

#include "../engine.h"

// Initializes the gfx subsystem in interlaced mode
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode);

// Starts a new gfx frame
int16 Gfx_BeginFrame();

// Submits all the OTs and finishes the frame's rendering to the current buffer
int16 Gfx_EndFrame();

// Shutdown the gfx subsystem
int16 Gfx_Shutdown();

// Updates clear color
void Gfx_SetClearColor(CVECTOR* i_color);

#endif // GFX_H_INC
