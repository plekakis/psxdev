#ifndef GFX_H_INC
#define GFX_H_INC

#include "../engine.h"
#include "../core/core.h"
#include "../core/core_allocators.h"
#include "../util/util.h"
#include "../time/time.h"
#include "gfx_scratch.h"

#define GFX_NUM_BUFFERS (2)

#define OT_LENGTH (10)
#define OT_ENTRIES (1 << OT_LENGTH)

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

// Primitive flags, used in various occasions
typedef enum
{
	PRIM_FLAG_NONE			= 1 << 0,
	PRIM_FLAG_SEMI_TRANS	= 1 << 1
}PrimFlags;

// Mode flags, used in DR_TPAGE and DR_MODE modifiers
typedef enum
{
	MODE_FLAG_NONE				= 1 << 0,
	MODE_FLAG_DITHERING			= 1 << 1,
	MODE_FLAG_DRAW_IN_DISP_AREA = 1 << 2
}ModeFlags;

// Blend rate
// BLEND_RATE_AVG			: 0.5 x Back + 0.5 x Forward
// BLEND_RATE_ADD			: 1.0 x Back + 1.0 x Forward
// BLEND_RATE_SUB			: 1.0 x Back - 1.0 x Forward
// BLEND_RATE_ADD_QUARTER   : 1.0 x Back + 0.25 x Forward
typedef enum
{
	BLEND_RATE_AVG = 0,
	BLEND_RATE_ADD = 1,
	BLEND_RATE_SUB = 2,
	BLEND_RATE_ADD_QUARTER = 3
}BlendRate;

// Refresh rate (NTSC values)
typedef enum
{
	REFRESH_60_HZ = 0,
	REFRESH_30_HZ = 2,
	REFRESH_20_HZ = 3,
	REFRESH_15_HZ = 4
}RefreshMode;

// Texture mode
typedef enum
{
	TEXTURE_MODE_CLUT_4BIT = 0,
	TEXTURE_MODE_CLUT_8BIT = 1,
	TEXTURE_MODE_16BIT = 2,
	TEXTURE_MODE_24BIT = 3
}TextureMode;

// Division count, maps to DivisionParams distance array
typedef enum
{
	DIVMODE_2x2		 = 4,
	DIVMODE_4x4		 = 3,
	DIVMODE_8x8		 = 2,
	DIVMODE_16x16	 = 1,
	DIVMODE_32x32	 = 0,
	DIVMODE_COUNT	 = (DIVMODE_2x2+1)
}DivisionMode;

// Polygon division parameters, such as lod distance and division count
// Distances must be in consecutive and incremental order (no per primitive distance sorting is performed) and higher lods (like 32x32, for example)
// must have lower distances than lower lods (like 2x2).
// Example distances:
// distances[DIVMODE_32x32] = 10;
// distances[DIVMODE_16x16] = 20;
// distances[DIVMODE_8x8] = 30;
// distances[DIVMODE_4x4] = 40;
// distances[DIVMODE_2x2] = 50;
//
// Gaps cannot exist, although not all division lods need to be used. Highest lod values can be ignored (left 0) and the next compatible range will be picked.
// Thus, the following is also valid:
//
// distances[DIVMODE_8x8] = 30;
// distances[DIVMODE_4x4] = 40;
// distances[DIVMODE_2x2] = 50;
//
typedef struct
{
	uint8 m_distances[DIVMODE_COUNT];	// Corresponds to the 5 levels of GTE polygon division in reverse order.
}DivisionParams;

// Used to specify a tpage using getTPage
typedef struct
{
	uint16		m_tpageX;
	uint16		m_tpageY;
	TextureMode	m_mode;
}TPageAddress;

// Used to specify a clut using getClut
typedef struct
{
	uint16		m_clutX;
	uint16		m_clutY;
}ClutAddress;

// 2D batches
typedef struct
{
	void*		m_baseAddress;
	void*		m_currentAddress;
	uint32		m_sizeInBytes;
	uint8		m_prevPrimSize;
}Batch2D;

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
	uint8 u, v;
	uint16 pad;
}TVECTOR;

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
	TVECTOR uv0, uv1, uv2;
	SVECTOR n0;
	CVECTOR c0;
}PRIM_FT3;

typedef struct
{
	SVECTOR v0, v1, v2;
	SVECTOR n0, n1, n2;
	CVECTOR c0, c1, c2;
	TVECTOR uv0, uv1, uv2;
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
	RS_PERSP		 = 1 << 0,
	RS_FOG			 = 1 << 1,
	RS_LIGHTING		 = 1 << 2,
	RS_TEXTURING	 = 1 << 3,
	RS_BACKFACE_CULL = 1 << 4,
	RS_DIVISION		 = 1 << 5
}RENDERSTATE;

// Initializes the gfx subsystem (interlaced is automatically chosen for high-resolution modes)
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode, uint8 i_refreshMode, uint32 i_gfxScratchSizeInBytes);

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
int16 Gfx_BeginFrame(TimeMoment* o_cpuTime);

// Submits all the OTs and finishes the frame's rendering to the current buffer
int16 Gfx_EndFrame(TimeMoment* o_cpuTime, TimeMoment* o_cpuTimeVsync);

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

// Sets the texture offset & scale. The final coordinate must be within [0, 255] range
void Gfx_SetTextureScaleOffset(uint8 i_scaleU, uint8 i_scaleV, uint8 i_offsetU, uint8 i_offsetV);

// Gets the current texture offset & scale.
void Gfx_GetTextureScaleOffset(uint8* o_scaleU, uint8* o_scaleV, uint8* o_offsetU, uint8* o_offsetV);

// Sets current texture description
void Gfx_SetTexture(TextureMode i_textureMode, BlendRate i_blendRate, TPageAddress* i_tpageAddress, ClutAddress* i_clutAddress);
void Gfx_SetTextureDirect(uint16 i_page, uint16 i_clut);

// Gets the current texture description
void Gfx_GetTexture(uint16* o_page, uint16* o_clut);

// Sets clear color
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

// Sets the polygon divison parameters
void Gfx_SetDivisionParams(DivisionParams* i_params);

// Gets the current polygon division parameters
void Gfx_GetDivisionParams(DivisionParams** o_params);

// Initialize the renderstate to sensible defaults
void Gfx_InitState();

// Add a primitive to the current OT
int16 Gfx_AddPrim(uint8 i_type, void* const i_prim);

// Add a primitive list to the current OT
int16 Gfx_AddPrims(uint8 i_type, void* const i_primArray, uint32 i_count);

// Add a billboarded point with NxM size
int16 Gfx_AddPointSprites(uint8 i_type, POINT_SPRITE* const i_pointArray, uint32 i_count);

// Add a size x size x size cube to the current OT
int16 Gfx_AddCube(uint8 i_type, uint32 i_size, CVECTOR* const i_colorArray, uint8 i_uvSize);

// Add a NxM plane to the current OT
int16 Gfx_AddPlane(uint8 i_type, uint32 i_width, uint32 i_height, CVECTOR* const i_colorArray, uint8 i_uvSize);

// Begin a batched 2D primitive submission
int16 Gfx_BeginBatch2D(Batch2D* o_batch, uint32 i_batchSizeInBytes);

// Add a TILE to the specified 2D batch
static int16 Gfx_Batch2D_AddTile(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, CVECTOR* const i_color, PrimFlags i_flags);

// Add a SPRT to the specified 2D batch
static int16 Gfx_Batch2D_AddSprite(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, TVECTOR* const i_uv, CVECTOR* const i_color, ClutAddress* i_clutAddress, PrimFlags i_flags);
static int16 Gfx_Batch2D_AddSpriteDirect(Batch2D* io_batch, DVECTOR* const i_position, DVECTOR* const i_size, TVECTOR* const i_uv, CVECTOR* const i_color, uint16 i_clut, PrimFlags i_flags);

// Add a LINE_F2 to the specified 2D batch
static int16 Gfx_Batch2D_AddLineF(Batch2D* io_batch, DVECTOR* const i_start, DVECTOR* const i_end, CVECTOR* const i_color, PrimFlags i_flags);

// Add a LINE_G2 to the specified 2D batch
static int16 Gfx_Batch2D_AddLineG(Batch2D* io_batch, DVECTOR* const i_start, DVECTOR* const i_end, CVECTOR* const i_startColor, CVECTOR* const i_endColor, PrimFlags i_flags);

// Add a series of SPRT to the specified 2D batch, for text rendering
static int16 Gfx_Batch2D_AddString(Batch2D* io_batch, const char* i_string, DVECTOR* const i_position, DVECTOR* const i_maxSize, CVECTOR* i_color, uint16 i_clut, PrimFlags i_flags);

// Add a DR_MODE to the specified 2D batch (optionally being able to calculate a tpage)
static int16 Gfx_Batch2D_AddMode(Batch2D* io_batch, BlendRate i_blendRate, ModeFlags i_flags, RECT* i_textureWindow, TPageAddress* i_tpageAddress);
static int16 Gfx_Batch2D_AddModeDirect(Batch2D* io_batch, ModeFlags i_flags, RECT* i_textureWindow, uint16 i_tpage);

// Add a DR_TPAGE to the specified 2D batch (optionally being able to calculate a tpage)
static int16 Gfx_Batch2D_AddTPage(Batch2D* io_batch, BlendRate i_blendRate, ModeFlags i_flags, TPageAddress* i_tpageAddress);
static int16 Gfx_Batch2D_AddTPageDirect(Batch2D* io_batch, ModeFlags i_flags, uint16 i_tpage);

// End a batched 2D primitive submission
int16 Gfx_EndBatch2D(Batch2D* i_batch);

// Returns the framebuffer's default draw mode
void Gfx_GetDefaultDrawMode(DR_MODE* o_mode);

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


#include "gfx_batch2d.h"
#endif // GFX_H_INC
