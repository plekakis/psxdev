#include "gfx.h"

// Max buffer pages, this will never be anything else than 2.
#define MAX_BUFFERS (2)

#define BG_OT (0)
#define FG_OT (1)
#define OV_OT (2)
#define OT_MAX (OV_OT+1)

#define MAX_OT_LENGTH (1 << 12)
#define BG_OT_LENGTH (1 << 4)
#define FG_OT_LENGTH (MAX_OT_LENGTH)
#define OV_OT_LENGTH (1 << 2)

#define PACKET_SIZE (1024)

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV		m_drawEnv;
    DISPENV		m_dispEnv;

	// 3 Root OTs
	// Background, Foreground and Overlay
	GsOT		m_OT[OT_MAX];
	GsOT_TAG	m_OTTag[OT_MAX][MAX_OT_LENGTH];
	PACKET		m_GpuPacketArea[PACKET_SIZE];

	POLY_F4*	rect;
		
}FrameBuffer;

// Framebuffer resources
static FrameBuffer g_frameBuffers[MAX_BUFFERS];
static FrameBuffer* g_currentFrameBuffer = NULL;

static uint64 g_frameIndex = 0ul;

// Other
static CVECTOR g_clearColor;

///////////////////////////////////////////////////
int16 Gfx_Initialize(uint8 i_isHighResolution, uint8 i_mode)
{
    uint16 index = 0;

    // setup resolution based on tv mode and high resolution flag
    const uint16 resWidth = i_isHighResolution ? 640 : 320;
    const uint16 resHeight = i_isHighResolution ?
                                 ( (i_mode == MODE_PAL) ? 512 : 480) :
                                 ( (i_mode == MODE_PAL) ? 256 : 240) ;

    const uint16 centerX = resWidth / 2;
    const uint16 centerY = resHeight / 2;

	// Reset graphic subsystem and set PAL mode
	const uint16 int1 = 1 | (1 << 2);
	GsInitGraph(resWidth, resHeight, int1, 1, 0);
    SetVideoMode(i_mode);

	// Set debug mode (0:off, 1:monitor, 2:dump)
	SetGraphDebug(0);

    // Initialize geometry subsystem*/
	InitGeom();

	// set geometry origin as (width/2, height/2)
	SetGeomOffset(centerX, centerY);

	// distance to viewing-screen
	SetGeomScreen(centerX);

	// Setup all the buffer page resources
	for (index=0; index<MAX_BUFFERS; ++index)
    {
        // Always in interlaced mode
        g_frameBuffers[index].m_dispEnv.isinter = 1;
        // And default to 16bit
        g_frameBuffers[index].m_dispEnv.isrgb24 = 0;

        SetDefDrawEnv(&g_frameBuffers[index].m_drawEnv, 0,   0, resWidth, resHeight);
        SetDefDispEnv(&g_frameBuffers[index].m_dispEnv, 0,   0, resWidth, resHeight);

		// Setup root ordering tables
		//g_frameBuffers[index].m_OTTag[BG_OT] = (GsOT_TAG*)malloc3(sizeof(GsOT_TAG) * BG_OT_LENGTH);
		//g_frameBuffers[index].m_OTTag[FG_OT] = (GsOT_TAG*)malloc3(sizeof(GsOT_TAG) * FG_OT_LENGTH);
		//g_frameBuffers[index].m_OTTag[OV_OT] = (GsOT_TAG*)malloc3(sizeof(GsOT_TAG) * OV_OT_LENGTH);

		g_frameBuffers[index].m_OT[BG_OT].length = MAX_OT_LENGTH;//BG_OT_LENGTH;
		g_frameBuffers[index].m_OT[FG_OT].length = MAX_OT_LENGTH;//FG_OT_LENGTH;
		g_frameBuffers[index].m_OT[OV_OT].length = MAX_OT_LENGTH;//OV_OT_LENGTH;

		g_frameBuffers[index].m_OT[BG_OT].org = g_frameBuffers[index].m_OTTag[BG_OT];
		g_frameBuffers[index].m_OT[FG_OT].org = g_frameBuffers[index].m_OTTag[FG_OT];
		g_frameBuffers[index].m_OT[OV_OT].org = g_frameBuffers[index].m_OTTag[OV_OT];

		GsClearOt(0,0,&g_frameBuffers[index].m_OT[BG_OT]);
		GsClearOt(0,0,&g_frameBuffers[index].m_OT[FG_OT]);
		GsClearOt(0,0,&g_frameBuffers[index].m_OT[OV_OT]);

		g_frameBuffers[index].rect = (POLY_F4*)malloc3(sizeof(POLY_F4));
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
    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_BeginFrame(uint64* o_cputime)
{
    g_currentFrameBuffer = &g_frameBuffers[g_frameIndex & 1];
	
	// Set work address for ordering table drawing
	// I believe that this is for the higher level rendering functions and it may safely be removed if there is no use for them
	GsSetWorkBase((PACKET*)g_currentFrameBuffer->m_GpuPacketArea);
	//

	// Clear (reset OT linked list, not actual color clear)
	{
		uint8 index;
		for (index=0; index<OT_MAX; ++index)
		{
			//
			// TODO: offset and avg z must be set per OT (BG, FG, OV)
			// Using GsSortOt
			//
			GsClearOt(0, 0, &g_currentFrameBuffer->m_OT[index]);
		}
	}

	*o_cputime = VSync(-1);

    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndFrame(uint64* o_cputime)
{
	{
		POLY_F4 rect;
		uint16 topLeftX = 32;
		uint16 topLeftY = 32;
		uint16 bottomRightX = 300;
		uint16 bottomRightY = 300;

		SetPolyF4(&rect);
		setXY4(&rect, topLeftX,      topLeftY,
						bottomRightX,  topLeftY,
						topLeftX,      bottomRightY,
						bottomRightX,  bottomRightY);

		setRGB0(&rect, 255, 255, 0);

		AddPrim(&g_currentFrameBuffer->m_OTTag[BG_OT], &rect);

	}

	{
		POLY_F4 rect;
		uint16 topLeftX = 64;
		uint16 topLeftY = 64;
		uint16 bottomRightX = 200;
		uint16 bottomRightY = 200;

		SetPolyF4(&rect);
		setXY4(&rect, topLeftX,      topLeftY,
						bottomRightX,  topLeftY,
						topLeftX,      bottomRightY,
						bottomRightX,  bottomRightY);

		setRGB0(&rect, 0, 255, 0);

		AddPrim(&g_currentFrameBuffer->m_OTTag[OV_OT], &rect);

	}


	*o_cputime = VSync(-1);

    // VSync and update the drawing environment
    VSync(0);

    // We are in interlace mode by default, so initialize the drawing engine but preserve the contents of the framebuffer
    ResetGraph(3);
	
    PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
    PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

	// For interlaced modes, use ClearImage2, as the docs say it is faster
    ClearImage2(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);

	// Draw all the OT
	{
		uint8 index;
		for (index=0; index<OT_MAX; ++index)
		{
			GsDrawOt(&g_currentFrameBuffer->m_OT[index]);
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
		uint32 index = 0;
		for (index = 0; index < OT_MAX; ++index)
		{
			//free3(g_frameBuffers[index].m_OTTag[index]);
		}
	}

    return E_OK;
}

///////////////////////////////////////////////////
void Gfx_SetClearColor(CVECTOR* i_color)
{
    g_clearColor.r = i_color->r;
    g_clearColor.g = i_color->g;
    g_clearColor.b = i_color->b;
}
