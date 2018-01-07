#ifndef GFX_H_INC
#define GFX_H_INC

#include "../engine.h"
#include "gfx_scratch.h"

// Polies
typedef enum
{
	PRIM_TYPE_POLY_F3		= 0,
	PRIM_TYPE_POLY_FT3		= 1,
	PRIM_TYPE_POLY_G3		= 2,
	PRIM_TYPE_POLY_GT3		= 3,
	PRIM_TYPE_MAX			= PRIM_TYPE_POLY_GT3 + 1
}PRIM_TYPE;

typedef struct
{
	SVECTOR v0, v1, v2;
	CVECTOR c;
}PRIM_F3;

typedef struct
{
	SVECTOR v0, v1, v2;
	CVECTOR c0, c1, c2;
}PRIM_G3;

// OT
typedef enum
{
	OT_LAYER_BG		= 0,
	OT_LAYER_FG		= 1,
	OT_LAYER_OV		= 2,
	OT_LAYER_MAX	= (OT_LAYER_OV + 1)
}OT_LAYER;

// AddPrim flags
typedef enum
{
	PRIM_FLAG_PERSP	= 1 << 0
}PRIM_FLAGS;

// Initializes the gfx subsystem (interlaced is automatically chosen for high-resolution modes)
int16 Gfx_Initialize(uint8 i_isInterlaced, uint8 i_isHighResolution, uint8 i_mode);

// Gets the diplay width
uint16 Gfx_GetDisplayWidth();

// Gets the display height
uint16 Gfx_GetDisplayHeight();

// Gets the tv mode (NTSC/PAL)
uint8 Gfx_GetTvMode();

// Gets resolution mode
uint8 Gfx_IsHighResolution();

// Gets interlaced mode
uint8 Gfx_IsInterlaced();

// Gets the current framebuffer index
uint8 Gfx_GetFrameBufferIndex();

// Starts a new gfx frame
int16 Gfx_BeginFrame(uint64* o_cputime);

// Submits all the OTs and finishes the frame's rendering to the current buffer
int16 Gfx_EndFrame(uint64* o_cputime, uint64* o_cputimeVsync, uint64* o_gputime);

// Begins primitive submission to the specified OT layer
int16 Gfx_BeginSubmission(uint8 i_layer);

// Add a primitive to the current OT
int16 Gfx_AddPrim(uint8 i_type, void* i_prim, uint8 i_flags);

// Ends primitive submission
int16 Gfx_EndSubmission();

// Shutdown the gfx subsystem
int16 Gfx_Shutdown();

// Updates clear color
void Gfx_SetClearColor(CVECTOR* i_color);

#endif // GFX_H_INC
