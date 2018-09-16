#include "gfx.h"

#define MAX_BUFFERS (2)

#define PACKET_SIZE (1024)

uint32 g_dirtyFlags = DF_ALL;
uint32 g_primStrides[PRIM_TYPE_MAX];

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV		m_drawEnv;
    DISPENV		m_dispEnv;

	// 3 Root OTs
	// Background, Foreground and Overlay
	uint32		m_OT[OT_LAYER_MAX][MAX_OT_LENGTH];
}FrameBuffer;

// Framebuffer resources
FrameBuffer* g_frameBuffers;
FrameBuffer* g_currentFrameBuffer = NULL;

int32 g_frameIndex = 0ul;
	
// when high-resolution is selected, we switch to interlaced mode
uint8  g_bufferCount;
uint16 g_displayWidth;
uint16 g_displayHeight;
uint16 g_tvMode;
bool   g_isHighResolution;

// Other
CVECTOR g_clearColor;

// Current matrices
MATRIX g_defaultModelMatrix;
MATRIX g_defaultCameraMatrix;

MATRIX* g_modelMatrix;
MATRIX* g_cameraMatrix;

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
uint8 Gfx_GetTvMode()
{
	return g_tvMode;
}

///////////////////////////////////////////////////
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode)
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
	g_bufferCount = 2;
	g_frameBuffers = (FrameBuffer*)malloc3(sizeof(FrameBuffer) * g_bufferCount);

	// Reset graphic subsystem and set tv mode, gpu offset bit (2)
	{
		uint16 int1 = 0;

		// mask in the interlaced bit
		if (g_isHighResolution)
			int1 |= (1 << 0);

		GsInitGraph(g_displayWidth, g_displayHeight, int1, 1, 0);
		SetVideoMode(g_tvMode);
	}

	// Set debug mode (0:off, 1:monitor, 2:dump)
	SetGraphDebug(0);

    // Initialize geometry subsystem*/
	InitGeom();

	// set geometry origin as (width/2, height/2)
	SetGeomOffset(g_displayWidth / 2, g_displayHeight / 2);

	// distance to viewing-screen
	SetGeomScreen(g_displayWidth / 2);

	// Default clear color to light blue
	{
		CVECTOR clearColor;
		clearColor.r = 0;
		clearColor.g = 64;
		clearColor.b = 127;
		Gfx_SetClearColor(&clearColor);
	}

	// Setup all the buffer page resources
	for (index=0; index<g_bufferCount; ++index)
    {
        // Always in interlaced mode
        g_frameBuffers[index].m_dispEnv.isinter = g_isHighResolution;
        // And default to 16bit
        g_frameBuffers[index].m_dispEnv.isrgb24 = FALSE;
		
        SetDefDrawEnv(&g_frameBuffers[index].m_drawEnv, 0, (!i_isHighResolution) * (index * g_displayHeight), g_displayWidth, g_displayHeight);
        SetDefDispEnv(&g_frameBuffers[index].m_dispEnv, 0, (!i_isHighResolution) * ((1-index) * g_displayHeight), g_displayWidth, g_displayHeight);
    }

    SetDispMask(1);
	PutDrawEnv(&g_frameBuffers[0].m_drawEnv);
	PutDispEnv(&g_frameBuffers[0].m_dispEnv);
	
	Gfx_InitScratch(g_bufferCount);

	// Initialize the callbacks for primitive submission
	InitAddPrimCallbacks();
	InitAddPointSprCallbacks();
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
	Gfx_SetBackColor(32, 32, 32);

	SetDefaultMatrices();
	//

	// Load the debug font and set it to render text at almost the origin, top-left
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(8, g_isHighResolution ? 16 : 8, g_displayWidth, 64, 0, 512));

	SetRCnt(RCntCNT1, 2048, RCntIntr); // TODO: revisit docs about the timers
	StartRCnt(RCntCNT1);
    return E_OK;
}

///////////////////////////////////////////////////
uint8 Gfx_GetFrameBufferIndex()
{
	return g_frameIndex & 1;
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

	// ClearOTagR() clears OT in reversed order. This is natural
	// for 3D type applications, because the OT pointer to be linked
	// is simply related to Z value of the primitive. ClearOTagR()
	// is faster than ClearOTag because it uses hardware DMA channel to clear.
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

	*o_cputimeVsync = GetRCnt(RCntCNT1);

	// VSync and update the drawing environment
	VSync(0);

	if (g_isHighResolution)
	{
		// When using interlaced single buffer, all drawing have to be
		// finished in 1/60 sec. Therefore we have to reset the drawing
		// procedure at the timing of VSync by calling ResetGraph(1)
		// instead of DrawSync(0)
		ResetGraph(1);
		ClearImage(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);
	}
	else
	{
		DrawSync(0);
		
		PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
		PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

		ClearImage(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);
	}

	// Draw all the OT
	// Since ClearOTagR() clears the OT as reversed order, the top
	// pointer of the table is ot[OTSIZE-1]. Notice that drawing
	// start point is not ot[0] but ot[OTSIZE-1].
	{
		uint8 index;
		for (index=0; index<OT_LAYER_MAX; ++index)
		{
			DrawOTag(g_currentFrameBuffer->m_OT[index] + MAX_OT_LENGTH-1);
		}
	}
	
	++g_frameIndex;

	FntFlush(-1);
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
void Gfx_SetClearColor(CVECTOR* const i_color)
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


///////////////////////////////////////////////////
void PrepareMatrices(bool i_billboard)
{
	if (DF_CHK(DF_LIGHTS | DF_MATRICES) && (Gfx_GetRenderState() & RS_LIGHTING))
	{
		MATRIX lightRotMatrix;
		
		// Update the local color matrix
		SetColorMatrix(&g_rs.m_lightColors);
		
		// Update the light vector matrix
		MulMatrix0(&g_rs.m_lightVectors, g_modelMatrix, &lightRotMatrix);
		SetLightMatrix(&lightRotMatrix);
		
		DF_INV(DF_LIGHTS);
	}

	if (DF_CHK(DF_MATRICES) || i_billboard)
	{
		MATRIX  finalMat = *g_cameraMatrix;
		
		// Apply the camera rotation to the camera vector		
		VECTOR transformedCameraPosition;
		VECTOR cameraPosition = {-finalMat.t[0], -finalMat.t[1], -finalMat.t[2]};
		
		TransposeMatrix(&finalMat, &finalMat);
		ApplyMatrixLV(&finalMat, &cameraPosition, &transformedCameraPosition);
		TransMatrix(&finalMat, &transformedCameraPosition);
		
		// Multiply current camera rotation matrix with the model rotation matrix
		MulMatrix0(g_modelMatrix, &finalMat, &finalMat);
		
		// Translate by the model parallel transfer vector
		finalMat.t[0] += g_modelMatrix->t[0];
		finalMat.t[1] += g_modelMatrix->t[1];
		finalMat.t[2] += g_modelMatrix->t[2];
		
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
		SetTransMatrix(&finalMat);
		SetRotMatrix(&finalMat);
		
		DF_INV(DF_MATRICES);
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
		AddPrim(g_currentFrameBuffer->m_OT[g_currentSubmissionOTIndex] + otz, primmem);		
	}
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddCube(uint8 i_type, uint32 i_size, CVECTOR* const i_colorArray)
{	
	fncAddCube[i_type](i_colorArray, i_size);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_AddPlane(uint8 i_type, uint32 i_width, uint32 i_height, CVECTOR* const i_colorArray)
{	
	fncAddPlane[i_type](i_colorArray, i_width, i_height);
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_SetModelMatrix(MATRIX* const i_matrix)
{
	g_modelMatrix = i_matrix;
	DF_SET(DF_MATRICES);
}

///////////////////////////////////////////////////
int16 Gfx_SetCameraMatrix(MATRIX* const i_matrix)
{
	g_cameraMatrix = i_matrix;
	DF_SET(DF_MATRICES);
}

///////////////////////////////////////////////////
int16 Gfx_AddPrims(uint8 i_type, void* const i_primArray, uint32 i_count)
{
	uint32 i = 0;
	uint32 stride = g_primStrides[i_type];

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
			AddPrim(g_currentFrameBuffer->m_OT[g_currentSubmissionOTIndex] + otz, primmem);
		}
	}

	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndSubmission()
{
	g_currentSubmissionOTIndex = ~0;
	return E_OK;
}