#include "gfx.h"

// Max buffer pages, this will never be anything else than 2.
#define MAX_BUFFERS (2)

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV m_drawEnv;
    DISPENV m_dispEnv;
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

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));

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
int16 Gfx_BeginFrame()
{
    g_currentFrameBuffer = &g_frameBuffers[g_frameIndex & 1];

    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_EndFrame()
{
    // VSync and update the drawing environment
    VSync(0);
    // We are in interlace mode by default, so initialize the drawing engine but preserve the contents of the framebuffer
    ResetGraph(3);
	
    PutDrawEnv(&g_currentFrameBuffer->m_drawEnv);
    PutDispEnv(&g_currentFrameBuffer->m_dispEnv);

    ClearImage2(&g_currentFrameBuffer->m_drawEnv.clip, g_clearColor.r, g_clearColor.g, g_clearColor.b);

    ++g_frameIndex;
    return E_OK;
}

///////////////////////////////////////////////////
int16 Gfx_Shutdown()
{
    DrawSync(0);
    return E_OK;
}

///////////////////////////////////////////////////
void Gfx_SetClearColor(CVECTOR* i_color)
{
    g_clearColor.r = i_color->r;
    g_clearColor.g = i_color->g;
    g_clearColor.b = i_color->b;
}
