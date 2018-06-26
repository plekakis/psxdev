#include "gfx.h"

#define MAX_BUFFERS (2)

#define MAX_OT_LENGTH (1 << 14)
#define BG_OT_LENGTH (1 << 4)
#define FG_OT_LENGTH (MAX_OT_LENGTH)
#define OV_OT_LENGTH (1 << 2)

#define PACKET_SIZE (1024)

#include "gfx_prim_callbacks.c"
#include "gfx_renderstate.c"

uint32 g_primStrides[PRIM_TYPE_MAX];

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV		m_drawEnv;
    DISPENV		m_dispEnv;

	// 3 Root OTs
	// Background, Foreground and Overlay
	int32		m_OT[OT_LAYER_MAX][MAX_OT_LENGTH];
}FrameBuffer;

// Framebuffer resources
static FrameBuffer* g_frameBuffers;
static FrameBuffer* g_currentFrameBuffer = NULL;

static uint32 g_frameIndex = 0ul;
	
// when high-resolution is selected, we switch to interlaced mode and single buffer
static bool   g_isInterlaced;
static uint8  g_bufferCount;

static uint16 g_displayWidth;
static uint16 g_displayHeight;
static uint16 g_tvMode;
static bool   g_isHighResolution;

// Other
static CVECTOR g_clearColor;

// Current matrices
static MATRIX g_defaultModelMatrix;
static MATRIX g_defaultCameraMatrix;

static MATRIX* g_modelMatrix;
static MATRIX* g_cameraMatrix;

// Dirty flags
typedef enum
{
	DF_MATRICES = 1 << 0,
	DF_ALL		= ~0
}DIRTYFLAGS;
static uint32 g_dirtyFlags = DF_ALL;
#define DF_CHK(x) ((g_dirtyFlags & (x)) != 0)
#define DF_SET(x) g_dirtyFlags |= (x)
#define DF_INV(x) g_dirtyFlags &= ~(x)

void SetDefaultMatrices()
{
	// Model
	{
		SVECTOR rotation = {0,0,0};
		VECTOR	position = {0,0,0};

		RotMatrix(&rotation, &g_defaultModelMatrix);
		TransMatrix(&g_defaultModelMatrix, &position);

		g_modelMatrix = &g_defaultModelMatrix;
	}
	// Camera
	{
		SVECTOR rotation = {0,0,0};
		VECTOR	position = {0,0,0};

		RotMatrix(&rotation, &g_defaultCameraMatrix);
		TransMatrix(&g_defaultCameraMatrix, &position);

		g_cameraMatrix = &g_defaultCameraMatrix;
	}

	DF_SET(DF_MATRICES);
}

// Timing
#define RCntIntr      0x1000            /*Interrupt mode*/



///////////////////////////////////////////////////
uint16 Gfx_GetDisplayWidth()
{
	return g_displayWidth;
}

///////////////////////////////////////////////////
uint16 Gfx_GetDisplayHeight()
{
	return g_displayHeight;
}

///////////////////////////////////////////////////
uint8 Gfx_IsHighResolution()
{
	return g_isHighResolution;
}

///////////////////////////////////////////////////
uint8 Gfx_IsInterlaced()
{
	return g_isInterlaced;
}

///////////////////////////////////////////////////
uint8 Gfx_GetTvMode()
{
	return g_tvMode;
}

///////////////////////////////////////////////////
int16 Gfx_Initialize(uint8 i_isInterlaced, uint8 i_isHighResolution, uint8 i_mode)
{
    uint16 index = 0;

    // setup resolution based on tv mode and high resolution flag
    g_displayWidth = i_isHighResolution ? 640 : 320;
    g_displayHeight = i_isHighResolution ?
                                 ( (i_mode == MODE_PAL) ? 512 : 480) :
                                 ( (i_mode == MODE_PAL) ? 256 : 240) ;
	g_tvMode = i_mode;
	g_isHighResolution = i_isHighResolution;
	
	// Buffer count & allocate framebuffers
	// (Single buffer in high-resolution, interlaced mode)
	g_bufferCount = (i_isHighResolution | i_isInterlaced) ? 1 : 2;
	g_isInterlaced = i_isHighResolution | i_isInterlaced;

	g_frameBuffers = (FrameBuffer*)malloc3(sizeof(FrameBuffer) * g_bufferCount);

	// Reset graphic subsystem and set tv mode, gpu offset bit (2)
	GsInitGraph(g_displayWidth, g_displayHeight, g_isInterlaced | (1 << 2), 1, 0);
    SetVideoMode(g_tvMode);

	// Set debug mode (0:off, 1:monitor, 2:dump)
	SetGraphDebug(0);

    // Initialize geometry subsystem*/
	InitGeom();

	// set geometry origin as (width/2, height/2)
	SetGeomOffset(g_displayWidth / 2, g_displayHeight / 2);

	// distance to viewing-screen
	SetGeomScreen(g_displayWidth / 2);

	// Setup all the buffer page resources
	for (index=0; index<g_bufferCount; ++index)
    {
        // Always in interlaced mode
        g_frameBuffers[index].m_dispEnv.isinter = g_isInterlaced;
        // And default to 16bit
        g_frameBuffers[index].m_dispEnv.isrgb24 = 0;

        SetDefDrawEnv(&g_frameBuffers[index].m_drawEnv, 0, (index * g_displayHeight), g_displayWidth, g_displayHeight);
        SetDefDispEnv(&g_frameBuffers[index].m_dispEnv, 0, (index * g_displayHeight), g_displayWidth, g_displayHeight);
    }

    SetDispMask(1);

    // Default clear color to light blue
    {
        CVECTOR clearColor;
        clearColor.r = 0;
        clearColor.g = 64;
        clearColor.b = 127;
        Gfx_SetClearColor(&clearColor);
    }
	
	Gfx_InitScratch(g_bufferCount);

	// Initialize the callbacks for primitive submission
	InitAddPrimCallbacks();
	InitAddCubeCallbacks();
	InitAddPlaneCallbacks();

	//
	// Populate the strides for the primitive types
	g_primStrides[PRIM_TYPE_POLY_F3] = sizeof(PRIM_F3);
	g_primStrides[PRIM_TYPE_POLY_FT3] = sizeof(PRIM_FT3);
	g_primStrides[PRIM_TYPE_POLY_G3] = sizeof(PRIM_G3);
	g_primStrides[PRIM_TYPE_POLY_GT3] = sizeof(PRIM_GT3);
	// add more here
	//

	// Default renderstate & matrices
	Gfx_SetRenderState(RS_PERSP);

	SetDefaultMatrices();
	//

	// Load the debug font and set it to render text at almost the origin, top-left
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(8, 8, g_displayWidth, 64, 0, 512));

	SetRCnt(RCntCNT1, 512000, RCntIntr);
	StartRCnt(RCntCNT1);
    return E_OK;
}

///////////////////////////////////////////////////
uint8 Gfx_GetFrameBufferIndex()
{
	return g_isInterlaced ? 0 : (g_frameIndex & 1);
}

///////////////////////////////////////////////////
int16 Gfx_BeginFrame(uint32* o_cputime)
{
	// Pick the next framebuffer
	const uint8 frameBufferIndex = Gfx_GetFrameBufferIndex();
	g_currentFrameBuffer = &g_frameBuffers[frameBufferIndex];
 
	ResetRCnt(RCntCNT1);
	*o_cputime = GetRCnt(RCntCNT1);

	// Reset scratch
	Gfx_ResetScratch(frameBufferIndex);
	
	// Clear (reset OT linked list, not actual color clear)
	{
		uint8 index;
		for (index=0; index<OT_LAYER_MAX; ++index)
		{
			ClearOTagR(g_currentFrameBuffer->m_OT[index], MAX_OT_LENGTH);
		}
	}	

    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndFrame(uint32* o_cputime, uint32* o_cputimeVsync, uint32* o_gputime)
{
	*o_cputime = GetRCnt(RCntCNT1);

	// sync in non-interlaced mode, so we wait for backbuffer completion
	if (!g_isInterlaced)
	{
		DrawSync(0);
	}

    // VSync and update the drawing environment
    VSync(0);
	
	*o_cputimeVsync = GetRCnt(RCntCNT1);

    PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
	PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

	// For interlaced modes, use ClearImage2, as the docs say it is faster
	// Also, initialize the drawing engine but preserve the contents of the framebuffer
	if (g_isInterlaced)
	{
		ResetGraph(3);
		ClearImage2(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);
	}
	else
	{		
		ClearImage(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);
	}

	// Draw all the OT
	{
		uint8 index;
		for (index=0; index<OT_LAYER_MAX; ++index)
		{
			DrawOTag(g_currentFrameBuffer->m_OT[index] + MAX_OT_LENGTH-1);
		}
	}
	
    ++g_frameIndex;
		
    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Shutdown()
{
    DrawSync(0);

	{
		uint8 index;
		for (index=0; index<g_bufferCount; ++index)
		{
			Gfx_FreeScratch(index);
		}
	}

	free3(g_frameBuffers);
	g_frameBuffers = NULL;
    return E_OK;
}

///////////////////////////////////////////////////
void Gfx_SetClearColor(CVECTOR* i_color)
{
    g_clearColor.r = i_color->r;
    g_clearColor.g = i_color->g;
    g_clearColor.b = i_color->b;
}

///////////////////////////////////////////////////
// PRIMITIVE SUBMISSION
///////////////////////////////////////////////////
uint8	g_currentSubmissionOTIndex = ~0;

///////////////////////////////////////////////////
int16 Gfx_BeginSubmission(uint8 i_layer)
{
	if (g_currentSubmissionOTIndex != i_layer)
	{
		g_currentSubmissionOTIndex = i_layer;
		return E_OK;
	}

	return E_SUBMISSION_ERROR;
}

bool g_canSetMatrices = TRUE;

void PrepareMatrices()
{
	if (g_canSetMatrices && DF_CHK(DF_MATRICES))
	{
		MATRIX  finalMat = *g_cameraMatrix;

		//
		// TODO:
		// The following doesn't seem to be a perspective correct transformation.
		// It probably needs the RotTransPersp and applied before vertex transformation
		// Investigate if we can do it here instead, as it's convenient.
		//

		// Apply the camera rotation to the camera vector
		{
			VECTOR transformedCameraPosition;
			const VECTOR cameraPosition = {finalMat.t[0], finalMat.t[1], finalMat.t[2]};

			TransposeMatrix(&finalMat, &finalMat);
			ApplyMatrixLV(&finalMat, &cameraPosition, &transformedCameraPosition);
			TransMatrix(&finalMat, &transformedCameraPosition);
		}

		// Invert the transformation
		{
			finalMat.t[0] *= -1;
			finalMat.t[1] *= -1;
			finalMat.t[2] *= -1;
		
			// Translate by the model parallel transfer vector
			finalMat.t[0] += g_modelMatrix->t[0];
			finalMat.t[1] += g_modelMatrix->t[1];
			finalMat.t[2] += g_modelMatrix->t[2];
		}

		// Multiply current camera rotation matrix with the model rotation matrix
		{
			MulMatrix0(g_modelMatrix, &finalMat, &finalMat);
		}
		
		// Update current rotation and translation matrices
		{
			SetRotMatrix(&finalMat);
			SetTransMatrix(&finalMat);
		}

		DF_INV(DF_MATRICES);
	}
}

///////////////////////////////////////////////////
int16 Gfx_AddPrim(uint8 i_type, void* i_prim)
{
	int32 otz = 0;
	void* primmem = NULL;
	
	PrepareMatrices();

	primmem = fncAddPrim[i_type](i_prim, &otz);
	if (primmem)
	{
		AddPrim(g_currentFrameBuffer->m_OT[g_currentSubmissionOTIndex] + otz, primmem);		
	}
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddCube(uint8 i_type, uint32 i_size, CVECTOR* i_colorArray)
{	
	PrepareMatrices();
	fncAddCube[i_type](i_colorArray, i_size);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddPlane(uint8 i_type, uint32 i_width, uint32 i_height, CVECTOR* i_colorArray)
{	
	PrepareMatrices();
	fncAddPlane[i_type](i_colorArray, i_width, i_height);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_SetModelMatrix(MATRIX* i_matrix)
{
	g_modelMatrix = i_matrix;
	DF_SET(DF_MATRICES);
}

///////////////////////////////////////////////////
int16 Gfx_SetCameraMatrix(MATRIX* i_matrix)
{
	g_cameraMatrix = i_matrix;
	DF_SET(DF_MATRICES);
}

///////////////////////////////////////////////////
int16 Gfx_AddPrims(uint8 i_type, void* i_primArray, uint32 i_count)
{
	uint32 i = 0;
	uint32 stride = g_primStrides[i_type];

	PrepareMatrices();
	
	g_canSetMatrices = FALSE;

	for (i=0; i<i_count; ++i)
	{			
		void* prim = i_primArray + stride * i;
		Gfx_AddPrim(i_type, prim);
	}
	
	g_canSetMatrices = TRUE;

	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndSubmission()
{
	g_currentSubmissionOTIndex = ~0;
	return E_OK;
}