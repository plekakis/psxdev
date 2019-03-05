#include "gfx.h"
#include "../util/util.h"

#define PACKET_SIZE (1024)

uint16 g_dirtyFlags = DF_ALL;
uint8 g_primStrides[PRIM_TYPE_MAX];

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV		m_drawEnv;
    DISPENV		m_dispEnv;

	DR_MODE		m_defaultMode;
	// 3 Root OTs
	// Background, Foreground and Overlay
	uint32		m_OT[OT_LAYER_MAX][OT_ENTRIES];
}FrameBuffer;

// Framebuffer resources
FrameBuffer g_frameBuffers[GFX_NUM_BUFFERS];
FrameBuffer* g_currentFrameBuffer = NULL;

uint8	g_currentSubmissionOTIndex = 0xf;

// when high-resolution is selected, we switch to interlaced mode
typedef struct
{
	uint32 m_displayWidth : 10;
	uint32 m_displayHeight : 10;
	uint32 m_frameIndex : 2;
	uint32 m_refreshMode : 3;
	uint32 m_tvMode : 1;
	uint32 m_isHighResolution : 1;
}FrameData;

FrameData g_dispProps;

// Current matrices
MATRIX* g_modelMatrix;
MATRIX* g_cameraMatrix;
MATRIX  g_currentCameraMatrix;

///////////////////////////////////////////////////
void SetDefaultMatrices()
{
	// Model
	{
		SVECTOR rotation = {0,0,0};
		VECTOR	position = {0,0,0};

		RotMatrix(&rotation, g_modelMatrix);
		TransMatrix(g_modelMatrix, &position);
	}
	// Camera
	{
		SVECTOR rotation = {0,0,0};
		VECTOR	position = {0,0,0};

		RotMatrix(&rotation, g_cameraMatrix);
		TransMatrix(g_cameraMatrix, &position);
	}

	DF_SET(DF_CAMERA_MATRIX | DF_MODEL_MATRIX);
}

///////////////////////////////////////////////////
uint32* Gfx_GetCurrentOT()
{
	VERIFY_ASSERT( (g_currentSubmissionOTIndex != 0xf), "Gfx_GetCurrentOT: Submission OT index hasn't been set, has BeginSubmission been called (Index: %u)?", g_currentSubmissionOTIndex);
	return g_currentFrameBuffer->m_OT[g_currentSubmissionOTIndex];
}

///////////////////////////////////////////////////
uint16 Gfx_GetDisplayWidth()
{
	return g_dispProps.m_displayWidth;
}

///////////////////////////////////////////////////
uint16 Gfx_GetDisplayHeight()
{
	return g_dispProps.m_displayHeight;
}

///////////////////////////////////////////////////
uint8 Gfx_IsHighResolution()
{
	return g_dispProps.m_isHighResolution;
}

///////////////////////////////////////////////////
uint8 Gfx_GetTvMode()
{
	return g_dispProps.m_tvMode;
}

///////////////////////////////////////////////////
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode, uint8 i_refreshMode, uint32 i_gfxScratchSizeInBytes)
{
    uint16 index = 0;

    // setup resolution based on tv mode and high resolution flag
    g_dispProps.m_displayWidth = i_isHighResolution ? 640 : 320;
	g_dispProps.m_displayHeight = i_isHighResolution ?
                                 ( (i_mode == MODE_PAL) ? 512 : 480) :
                                 ( (i_mode == MODE_PAL) ? 256 : 240) ;
	g_dispProps.m_tvMode = i_mode;
	g_dispProps.m_isHighResolution = i_isHighResolution;
	g_dispProps.m_refreshMode = i_refreshMode;

	// Reset graphic subsystem and set tv mode, gpu offset bit (2)
	{
		uint16 int1 = 0;

		// mask in the interlaced bit
		if (Gfx_IsHighResolution())
			int1 |= 1;

		GsInitGraph(Gfx_GetDisplayWidth(), Gfx_GetDisplayHeight(), int1, 1, 0);
	}

	// Set debug mode (0:off, 1:monitor, 2:dump)
#if CONFIG_DEBUG
	SetGraphDebug(1);
#else
	SetGraphDebug(0);
#endif // CONFIG_DEBUG

    // Initialize geometry subsystem
	InitGeom();

	// Set video mode PAL / NTSC
	SetVideoMode(g_dispProps.m_tvMode);
	TTY_OUT((g_dispProps.m_tvMode == MODE_PAL) ? "Setting PAL" : "Setting NTSC");



	// set geometry origin as (width/2, height/2)
	SetGeomOffset(Gfx_GetDisplayWidth() / 2, Gfx_GetDisplayHeight() / 2);

	// distance to viewing-screen
	SetGeomScreen(Gfx_GetDisplayWidth() / 2);
			
	Util_MemZero(g_frameBuffers, sizeof(g_frameBuffers));
	// Setup all the buffer page resources
	for (index=0; index<GFX_NUM_BUFFERS; ++index)
    {
		uint32 xoffset = 0;
		FrameBuffer* buffer = &g_frameBuffers[index];

        // Always in interlaced mode
        g_frameBuffers[index].m_dispEnv.isinter = Gfx_IsHighResolution();
        // And default to 16bit
        g_frameBuffers[index].m_dispEnv.isrgb24 = FALSE;
		
        SetDefDrawEnv(&buffer->m_drawEnv, 0, (!i_isHighResolution) * (index * Gfx_GetDisplayHeight()), Gfx_GetDisplayWidth(), Gfx_GetDisplayHeight());
        SetDefDispEnv(&buffer->m_dispEnv, 0, (!i_isHighResolution) * ((1-index) * Gfx_GetDisplayHeight()), Gfx_GetDisplayWidth(), Gfx_GetDisplayHeight());

		// Screen starting position must be modified for PAL.
		// Y must be moved down by 16 lines.
		if (g_dispProps.m_tvMode == MODE_PAL)
		{
#if TARGET_EMU
			// NO$PSX requires this to align correctly
			xoffset = i_isHighResolution ? -8 : -2;
#endif // TARGET_EMU
			setRECT(&buffer->m_dispEnv.screen, xoffset, 16, 256, 256);
		}
		else
		{
#if TARGET_EMU
			// NO$PSX requires this to align correctly
			xoffset = i_isHighResolution ? -3 : 1;
#endif // TARGET_EMU
			setRECT(&buffer->m_dispEnv.screen, xoffset, 8, 256, 256);
		}

		// Update the draw mode member; this will be used as a default when Entering/Leaving a gfx batch.
		setDrawMode(&buffer->m_defaultMode, buffer->m_drawEnv.dfe, buffer->m_drawEnv.dtd, buffer->m_drawEnv.tpage, &buffer->m_drawEnv.tw);
    }

    SetDispMask(1);
	PutDrawEnv(&g_frameBuffers[0].m_drawEnv);
	PutDispEnv(&g_frameBuffers[0].m_dispEnv);

	Gfx_InitScratch(i_gfxScratchSizeInBytes);

	// Initialize the callbacks for primitive submission
	InitPrimCallbacks();

	//
	// Populate the strides for the primitive types
	g_primStrides[PRIM_TYPE_POLY_F3] = sizeof(PRIM_F3);
	g_primStrides[PRIM_TYPE_POLY_FT3] = sizeof(PRIM_FT3);
	g_primStrides[PRIM_TYPE_POLY_G3] = sizeof(PRIM_G3);
	g_primStrides[PRIM_TYPE_POLY_GT3] = sizeof(PRIM_GT3);
	// add more here
	//

	// Default renderstate & matrices
	Gfx_InitState();	
	SetDefaultMatrices();
	//

	g_dispProps.m_frameIndex = 0;

	// Load the debug font and set it to render text at almost the origin, top-left
#if !CONFIG_FINAL
	{
		// Place it near at the top right corner of the framebuffer, clut following just below.
		const uint8  tw = s_debug_font_width;
		const uint8  th = s_debug_font_height;
		const uint32 tx = 1024-tw;
		const uint32 ty = 0;

		s_debugfontClut = LoadClut2(s_debug_font, tx, ty + th);
		s_debugFontTPage = LoadTPage(s_debug_font + 0x80, 0, 0, tx, ty, tw, th);
	}
#endif // !CONFIG_FINAL
		
	SetRCnt(RCntCNT1, 4096, RCntMdINTR);
	StartRCnt(RCntCNT1);
    return E_OK;
}

///////////////////////////////////////////////////
uint8 Gfx_GetFrameBufferIndex()
{
	return g_dispProps.m_frameIndex & 1;
}

///////////////////////////////////////////////////
void Gfx_GetDefaultDrawMode(DR_MODE* o_mode)
{
	VERIFY_ASSERT(o_mode, "Gfx_GetDefaultDrawMode:  Null output pointer");
	*o_mode = g_frameBuffers[Gfx_GetFrameBufferIndex()].m_defaultMode;
}

///////////////////////////////////////////////////
int16 Gfx_BeginFrame(uint16* o_cputime)
{
	// Pick the next framebuffer
	const uint8 frameBufferIndex = Gfx_GetFrameBufferIndex();
	g_currentFrameBuffer = &g_frameBuffers[frameBufferIndex];
 
	// Reset camera and model matrices
	Gfx_InitState();
	SetDefaultMatrices();

	ResetRCnt(RCntCNT1);
	*o_cputime = (uint16)GetRCnt(RCntCNT1);

	// Reset scratch
	Gfx_ResetScratch();

	// Clear any data used by the primitive submission system
	BeginPrimSubmission();

	// ClearOTagR() clears OT in reversed order. This is natural
	// for 3D type applications, because the OT pointer to be linked
	// is simply related to Z value of the primitive. ClearOTagR()
	// is faster than ClearOTag because it uses hardware DMA channel to clear.
	{
		uint8 index;
		for (index=0; index<OT_LAYER_MAX; ++index)
		{
			ClearOTagR(g_currentFrameBuffer->m_OT[index], OT_ENTRIES);
		}
	}

    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndFrame(uint16* o_cputime, uint16* o_cputimeVsync, uint16* o_gputime)
{
	CVECTOR clearColor;
	Gfx_GetClearColor(&clearColor.r, &clearColor.g, &clearColor.b);

	*o_cputime = (uint16)GetRCnt(RCntCNT1);
	
	// VSync and update the drawing environment
	VSync(g_dispProps.m_refreshMode);

	*o_cputimeVsync = (uint16)GetRCnt(RCntCNT1);
	
	if (Gfx_IsHighResolution())
	{
		// When using interlaced single buffer, all drawing have to be
		// finished in 1/60 sec. Therefore we have to reset the drawing
		// procedure at the timing of VSync by calling ResetGraph(1)
		// instead of DrawSync(0)
		ResetGraph(1);
		ClearImage2(&g_currentFrameBuffer->m_drawEnv.clip, clearColor.r, clearColor.g, clearColor.b);
		g_currentFrameBuffer->m_drawEnv.dfe = 0;
	}
	else
	{
		DrawSync(0);

		PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
		PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

		ClearImage(&g_currentFrameBuffer->m_drawEnv.clip, clearColor.r, clearColor.g, clearColor.b);
	}

	// Draw all the OT
	// Since ClearOTagR() clears the OT as reversed order, the top
	// pointer of the table is ot[OTSIZE-1]. Notice that drawing
	// start point is not ot[0] but ot[OTSIZE-1].
	{
		uint8 index;
		for (index=0; index<OT_LAYER_MAX; ++index)
		{
			DrawOTag(g_currentFrameBuffer->m_OT[index] + OT_ENTRIES-1);
		}
	}
	
	*o_gputime = (uint16)GetRCnt(RCntCNT1) - *o_cputimeVsync;
	g_dispProps.m_frameIndex = (g_dispProps.m_frameIndex + 1) % GFX_NUM_BUFFERS;
    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Shutdown()
{
    DrawSync(0);
	Gfx_FreeScratch();	
    return E_OK;
}

///////////////////////////////////////////////////
// PRIMITIVE SUBMISSION
///////////////////////////////////////////////////

///////////////////////////////////////////////////
int16 Gfx_BeginSubmission(uint8 i_layer)
{
	if (g_currentSubmissionOTIndex != i_layer)
	{
		g_currentSubmissionOTIndex = i_layer;		
	}
	return E_OK;
}


///////////////////////////////////////////////////
void PrepareMatrices(bool i_billboard)
{
	if (DF_CHK(DF_LIGHTS | DF_MODEL_MATRIX) && (Gfx_GetRenderState() & RS_LIGHTING))
	{
		MATRIX lightRotMatrix;
		
		// Update the local color matrix
		gte_SetColorMatrix(&g_rs.m_lightColors);
		
		// Update the light vector matrix
		gte_MulMatrix0(&g_rs.m_lightVectors, g_modelMatrix, &lightRotMatrix);
		
		gte_SetLightMatrix(&lightRotMatrix);
		
		DF_INV(DF_LIGHTS);
	}

	if (DF_CHK(DF_CAMERA_MATRIX))
	{
		g_currentCameraMatrix = *g_cameraMatrix;

		// Apply the camera rotation to the camera vector
		{
			VECTOR transformedCameraPosition;
			VECTOR scaleVec = { ONE, -ONE, ONE };
			
			// Invert the translation and transpose the 3x3 rotation matrix
			VECTOR cameraPosition = { -g_currentCameraMatrix.t[0], -g_currentCameraMatrix.t[1], -g_currentCameraMatrix.t[2] };

			// Bring to Y up
			ScaleMatrix(&g_currentCameraMatrix, &scaleVec);

			TransposeMatrix(&g_currentCameraMatrix, &g_currentCameraMatrix);

			// Multiply the cameraPosition by the rotation matrix so we get the correct transformed camera position
			ApplyMatrixLV(&g_currentCameraMatrix, &cameraPosition, &transformedCameraPosition);
						
			// Apply this translation to the camera matrix
			TransMatrix(&g_currentCameraMatrix, &transformedCameraPosition);						
		}

		DF_SET(DF_MODEL_MATRIX); // camera  matrix changed, we must re-compose the modelview matrix.
		DF_INV(DF_CAMERA_MATRIX);
	}

	if (DF_CHK(DF_MODEL_MATRIX) || i_billboard)
	{
		MATRIX finalMat;

		// Compose camera matrix with model
		gte_CompMatrix(&g_currentCameraMatrix, g_modelMatrix, &finalMat);

		if (i_billboard)
		{
			finalMat.m[0][0] = ONE;
			finalMat.m[0][1] = 0;
			finalMat.m[0][2] = 0;

			finalMat.m[1][0] = 0;
			finalMat.m[1][1] = ONE;
			finalMat.m[1][2] = 0;

			finalMat.m[2][0] = 0;
			finalMat.m[2][1] = 0;
			finalMat.m[2][2] = ONE;
		}

		// Update current rotation and translation matrices
		gte_SetTransMatrix(&finalMat);
		gte_SetRotMatrix(&finalMat);

		DF_INV(DF_MODEL_MATRIX);
	}
}

///////////////////////////////////////////////////
int16 Gfx_AddPrim(uint8 i_type, void* const i_prim)
{
	int32 otz = 0;
	void* primmem = NULL;
	
	PrepareMatrices(FALSE);

	primmem = fncAddPrim[i_type](i_prim, &otz);
	if (primmem)
	{
		addPrim(Gfx_GetCurrentOT() + otz, primmem);
	}
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddCube(uint8 i_type, uint32 i_size, CVECTOR* const i_colorArray, uint8 i_uvSize)
{	
	fncAddCube[i_type](i_colorArray, i_size, i_uvSize);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddPlane(uint8 i_type, uint32 i_width, uint32 i_height, CVECTOR* const i_colorArray, uint8 i_uvSize)
{	
	fncAddPlane[i_type](i_colorArray, i_width, i_height, i_uvSize);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_SetModelMatrix(MATRIX* const i_matrix)
{
	g_modelMatrix = i_matrix;
	DF_SET(DF_MODEL_MATRIX);
}

///////////////////////////////////////////////////
int16 Gfx_SetCameraMatrix(MATRIX* const i_matrix)
{
	g_cameraMatrix = i_matrix;
	DF_SET(DF_CAMERA_MATRIX);
}

///////////////////////////////////////////////////
int16 Gfx_AddPrims(uint8 i_type, void* const i_primArray, uint32 i_count)
{
	uint32 i = 0;
	uint32 stride = g_primStrides[i_type];

	VERIFY_ASSERT(i_primArray, "Gfx_AddPrims: Input primitive array cannot be null!");

	for (i=0; i<i_count; ++i)
	{			
		void* const prim = i_primArray + stride * i;
		Gfx_AddPrim(i_type, prim);
	}
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddPointSprites(uint8 i_type, POINT_SPRITE* const i_pointArray, uint32 i_count)
{
	uint32 i = 0;

	PrepareMatrices(TRUE);

	for (i = 0; i < i_count; ++i)
	{
		int32 otz;
		void* primmem = fncAddPointSpr[i_type](i_pointArray + i, &otz);
		
		if (primmem)
		{
			addPrim(Gfx_GetCurrentOT() + otz, primmem);
		}
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndSubmission()
{
	g_currentSubmissionOTIndex = 0xf;
	return E_OK;
}