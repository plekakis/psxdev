#ifndef GFX_H_INC
#define GFX_H_INC

#include "../engine.h"
#include "gfx_scratch.h"

#define MAX_OT_LENGTH (1 << 14)
#define BG_OT_LENGTH (1 << 4)
#define FG_OT_LENGTH (MAX_OT_LENGTH)
#define OV_OT_LENGTH (1 << 2)

// POLY sizes as u_longs
#define	POLY_F3_size	(sizeof(POLY_F3)+3)/4
#define	POLY_F4_size	(sizeof(POLY_F4)+3)/4
#define	POLY_FT3_size	(sizeof(POLY_FT3)+3)/4
#define POLY_FT4_size	(sizeof(POLY_FT4)+3)/4
#define	POLY_G3_size	(sizeof(POLY_G3)+3)/4
#define	POLY_G4_size	(sizeof(POLY_G4)+3)/4
#define	POLY_GT3_size	(sizeof(POLY_GT3)+3)/4
#define POLY_GT4_size	(sizeof(POLY_GT4)+3)/4

// Dirty flags
typedef enum
{
	DF_CAMERA_MATRIX = 1 << 0,
	DF_MODEL_MATRIX = 1 << 1,
	DF_LIGHTS = 1 << 2,
	DF_ALL = ~0
}DIRTYFLAGS;
extern uint16 g_dirtyFlags;
#define DF_CHK(x) ((g_dirtyFlags & (x)) != 0)
#define DF_SET(x) g_dirtyFlags |= (x)
#define DF_INV(x) g_dirtyFlags &= ~(x)

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
	SVECTOR n0;
	CVECTOR c0;
}PRIM_F3;

typedef struct
{
	SVECTOR v0, v1, v2;
	SVECTOR n0, n1, n2;
	CVECTOR c0, c1, c2;
}PRIM_G3;

typedef struct
{
	SVECTOR v0, v1, v2;
	SVECTOR n0;
	CVECTOR c0;
}PRIM_FT3;

typedef struct
{
	SVECTOR v0, v1, v2;
	SVECTOR n0, n1, n2;
	CVECTOR c0, c1, c2;
}PRIM_GT3;

// Point sprites are emulated on psx as billboarded rectangles of NxM size
typedef struct
{
	SVECTOR	p;
	CVECTOR c;

	uint16	width;
	uint16	height;
}POINT_SPRITE;

// OT
typedef enum
{
	OT_LAYER_BG		= 0,
	OT_LAYER_FG		= 1,
	OT_LAYER_OV		= 2,
	OT_LAYER_MAX	= (OT_LAYER_OV + 1)
}OT_LAYER;

// Renderstate flags
typedef enum
{
	RS_PERSP		= 1 << 0,
	RS_FOG			= 1 << 1,
	RS_LIGHTING		= 1 << 2,
	RS_TEXTURING	= 1 << 3
}RENDERSTATE;

// Initializes the gfx subsystem (interlaced is automatically chosen for high-resolution modes)
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode, uint32 i_gfxScratchSizeInBytes);

// Gets the diplay width
uint16 Gfx_GetDisplayWidth();

// Gets the display height
uint16 Gfx_GetDisplayHeight();

// Gets the tv mode (NTSC/PAL)
uint8 Gfx_GetTvMode();

// Gets resolution mode
uint8 Gfx_IsHighResolution();

// Gets the current framebuffer index
uint8 Gfx_GetFrameBufferIndex();

// Starts a new gfx frame
int16 Gfx_BeginFrame(uint16* o_cputime);

// Submits all the OTs and finishes the frame's rendering to the current buffer
int16 Gfx_EndFrame(uint16* o_cputime, uint16* o_cputimeVsync, uint16* o_gputime);

// Gets the current OT (based on the current OT layer)
uint32* Gfx_GetCurrentOT();

// Begins primitive submission to the specified OT layer
int16 Gfx_BeginSubmission(uint8 i_layer);

// Sets the model matrix to be currently used for rendering. This is a translation & rotation matrix
int16 Gfx_SetModelMatrix(MATRIX* const i_matrix);

// Sets the camera matrix to be currently used for rendering. This is a translation & rotation matrix
int16 Gfx_SetCameraMatrix(MATRIX* const i_matrix);

// Gets the renderstate flags
uint32 Gfx_GetRenderState();

// Invalidates the renderstate bit and returns the modified state
uint32 Gfx_InvalidateRenderState(uint32 i_state);

// Sets the renderstate bit and returns the modified state
uint32 Gfx_SetRenderState(uint32 i_state);

// Updates clear color
void Gfx_SetClearColor(uint8 i_red, uint8 i_green, uint8 i_blue);

// Gets clear color
void Gfx_GetClearColor(uint8* o_red, uint8* o_green, uint8* o_blue);

// Sets fog near/far distances
void Gfx_SetFogNearFar(uint32 i_near, uint32 i_far);

// Gets fog near/far distances
void Gfx_GetFogNearFar(uint32* o_near, uint32* o_far);

// Sets fog color
void Gfx_SetFogColor(uint8 i_red, uint8 i_green, uint8 i_blue);

// Gets fog color
void Gfx_GetFogColor(uint8* o_red, uint8* o_green, uint8* o_blue);

// Sets the back color
void Gfx_SetBackColor(uint8 i_red, uint8 i_green, uint8 i_blue);

// Gets the back color
void Gfx_GetBackColor(uint8* o_red, uint8* o_green, uint8* o_blue);

// Sets the light vector for the specified light index
void Gfx_SetLightVector(uint8 i_index, uint16 i_x, uint16 i_y, uint16 i_z);

// Sets the light color for the specified light index
void Gfx_SetLightColor(uint8 i_index, uint32 i_red, uint32 i_green, uint32 i_blue);

// Add a primitive to the current OT
int16 Gfx_AddPrim(uint8 i_type, void* const i_prim);

// Add a primitive list to the current OT
int16 Gfx_AddPrims(uint8 i_type, void* const i_primArray, uint32 i_count);

// Add a billboarded point with NxM size
int16 Gfx_AddPointSprites(uint8 i_type, POINT_SPRITE* const i_pointArray, uint32 i_count);

// Add a size x size x size cube to the current OT
int16 Gfx_AddCube(uint8 i_type, uint32 i_size, CVECTOR* const i_colorArray);

// Add a NxM plane to the current OT
int16 Gfx_AddPlane(uint8 i_type, uint32 i_width, uint32 i_height, CVECTOR* const i_colorArray);

// Ends primitive submission
int16 Gfx_EndSubmission();

// Shutdown the gfx subsystem
int16 Gfx_Shutdown();

#if !CONFIG_FINAL
typedef struct
{
	// F3
	uint16 m_primF3;
	uint16 m_primDivF3;
	uint16 m_primLitF3;
	uint16 m_primFogF3;

	// FT3
	uint16 m_primFT3;
	uint16 m_primDivFT3;
	uint16 m_primLitFT3;
	uint16 m_primFogFT3;

	// G3
	uint16 m_primG3;
	uint16 m_primDivG3;
	uint16 m_primLitG3;
	uint16 m_primFogG3;

	// GT3
	uint16 m_primGT3;
	uint16 m_primDivGT3;
	uint16 m_primLitGT3;
	uint16 m_primFogGT3;
}GfxPrimCounts;

// Get primitive counts that have been submitted to the OT
void Gfx_Debug_GetPrimCounts(GfxPrimCounts* o_counts);

#endif // !CONFIG_FINAL

#endif // GFX_H_INC
