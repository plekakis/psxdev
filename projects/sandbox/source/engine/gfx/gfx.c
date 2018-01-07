#include "gfx.h"

#define MAX_BUFFERS (2)

#define MAX_OT_LENGTH (1 << 12)
#define BG_OT_LENGTH (1 << 4)
#define FG_OT_LENGTH (MAX_OT_LENGTH)
#define OV_OT_LENGTH (1 << 2)

#define PACKET_SIZE (1024)

// Callbacks for primitive submission, one per type
void* (*fncAddPrim[PRIM_TYPE_MAX])(void*, uint64*);
void InitAddPrimCallbacks();

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV		m_drawEnv;
    DISPENV		m_dispEnv;

	// 3 Root OTs
	// Background, Foreground and Overlay
	uint64		m_OT[OT_LAYER_MAX][MAX_OT_LENGTH];
	PACKET		m_GpuPacketArea[PACKET_SIZE];		
}FrameBuffer;

// Framebuffer resources
static FrameBuffer* g_frameBuffers;
static FrameBuffer* g_currentFrameBuffer = NULL;

static uint64 g_frameIndex = 0ul;

// when high-resolution is selected, we switch to interlaced mode and single buffer
static uint8  g_isInterlaced = 0;
static uint8  g_bufferCount = 2;

static uint16 g_displayWidth;
static uint16 g_displayHeight;
static uint16 g_tvMode;
static uint8  g_isHighResolution;

// Other
static CVECTOR g_clearColor;

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
        clearColor.g = 127;
        clearColor.b = 255;
        Gfx_SetClearColor(&clearColor);
    }
	
	Gfx_InitScratch(g_bufferCount);

	// Initialize the callbacks for primitive submission
	InitAddPrimCallbacks();

	// Load the debug font and set it to render text at almost the origin, top-left
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(8, 8, 320, 64, 0, 512));

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
int16 Gfx_BeginFrame(uint64* o_cputime)
{
	// Pick the next framebuffer
	const uint8 frameBufferIndex = Gfx_GetFrameBufferIndex();
	g_currentFrameBuffer = &g_frameBuffers[frameBufferIndex];
 
	ResetRCnt(RCntCNT1);
	*o_cputime = GetRCnt(RCntCNT1);

	// Reset scratch
	Gfx_ResetScratch(frameBufferIndex);

	// Set work address for ordering table drawing
	// I believe that this is for the higher level rendering functions and it may safely be removed if there is no use for them
	GsSetWorkBase((PACKET*)g_currentFrameBuffer->m_GpuPacketArea);
	//

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
int16 Gfx_EndFrame(uint64* o_cputime, uint64* o_cputimeVsync, uint64* o_gputime)
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

	// If we are in interlace mode, initialize the drawing engine but preserve the contents of the framebuffer
	if (g_isInterlaced)
	{    
		ResetGraph(3);
	}
	
    PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
    PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

	// For interlaced modes, use ClearImage2, as the docs say it is faster
    ClearImage2(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);

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
void* AddPrim_POLY_3(void* i_prim, uint64* o_otz)
{
	uint64	p, flg, otz;
	uint32	isomote;
	PRIM_F3* prim = (PRIM_F3*)i_prim;
	POLY_F3* poly = (POLY_F3*)Gfx_Alloc(sizeof(POLY_F3), 4);
	
	SetPolyF3(poly);

	isomote = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
			(int64 *)&poly->x0, (int64 *)&poly->x1, (int64 *)&poly->x2,
			&p, &otz, &flg);

	if (isomote > 0)
	{
		*o_otz = otz;
		return poly;
	}
	return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_F3(void* i_prim, uint64* o_otz)
{
	uint64	p, flg, otz;
	uint32	isomote;
	PRIM_F3* prim = (PRIM_F3*)i_prim;	
	POLY_F3* poly = (POLY_F3*)Gfx_Alloc(sizeof(POLY_F3), 4);
	
	SetPolyF3(poly);

	isomote = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
			(int64 *)&poly->x0, (int64 *)&poly->x1, (int64 *)&poly->x2,
			&p, &otz, &flg);

	if (isomote > 0)
	{
		setRGB0(poly, prim->c.r, prim->c.g, prim->c.b);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_FT3(void* i_prim, uint64* o_otz)
{
	POLY_FT3* poly = (POLY_FT3*)i_prim;
	SetPolyFT3(poly);

	*o_otz = 0;
	return poly;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_G3(void* i_prim, uint64* o_otz)
{
	uint64	p, flg, otz;
	uint32	isomote;
	PRIM_G3* prim = (PRIM_G3*)i_prim;	
	POLY_G3* poly = (POLY_G3*)Gfx_Alloc(sizeof(POLY_G3), 4);
	
	SetPolyG3(poly);

	isomote = RotAverageNclip3(&prim->v0, &prim->v1, &prim->v2,
			(int64*)&poly->x0, (int64*)&poly->x1, (int64*)&poly->x2,
			&p, &otz, &flg);

	if (isomote > 0)
	{
		setRGB0(poly, prim->c0.r, prim->c0.g, prim->c0.b);
		setRGB1(poly, prim->c1.r, prim->c1.g, prim->c1.b);
		setRGB2(poly, prim->c2.r, prim->c2.g, prim->c2.b);

		*o_otz = otz;
		return poly;
	}
    return NULL;
}

///////////////////////////////////////////////////
void* AddPrim_POLY_GT3(void* i_prim, uint64* o_otz)
{
	POLY_GT3* poly = (POLY_GT3*)i_prim;
	SetPolyGT3(poly);

	*o_otz = 0;
	return poly;
}

///////////////////////////////////////////////////
void InitAddPrimCallbacks()
{
	// POLY
	fncAddPrim[PRIM_TYPE_POLY_F3] = &AddPrim_POLY_F3;
	fncAddPrim[PRIM_TYPE_POLY_FT3] = &AddPrim_POLY_FT3;
	fncAddPrim[PRIM_TYPE_POLY_G3] = &AddPrim_POLY_G3;
	fncAddPrim[PRIM_TYPE_POLY_GT3] = &AddPrim_POLY_GT3;
}

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

#define SCR_Z 512

///////////////////////////////////////////////////
int16 Gfx_AddPrim(uint8 i_type, void* i_prim, uint8 i_flags)
{
	static SVECTOR	ang  = { 0, 0, 0};	
	static VECTOR	vec  = {0, 0, SCR_Z};	
	
	MATRIX		rottrans;
	uint64 otz = 0;
	void* primmem = NULL;
	
	RotMatrix(&ang, &rottrans);	/* rotation*/
	TransMatrix(&rottrans, &vec);	/* translation*/
	
	/* set matrix*/
	SetRotMatrix(&rottrans);		/* rotation*/
	SetTransMatrix(&rottrans);	/* translation*/
		
	primmem = fncAddPrim[i_type](i_prim, &otz);
	if (primmem)

	{
		AddPrim(g_currentFrameBuffer->m_OT[g_currentSubmissionOTIndex] + otz, primmem);
	}
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndSubmission()
{
	g_currentSubmissionOTIndex = ~0;
	return E_OK;
}