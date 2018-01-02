
#ifndef ____HELPER_H____
#define ____HELPER_H____

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>
#include <libcd.h>

// ----------
// CONSTANTS
// ----------
#define PI 3.14159265359f

#define LOW_RES_X 320
#define LOW_RES_Y 240

#define HIGH_RES_X 640
#define HIGH_RES_Y 480

#define SCREEN_X HIGH_RES_X // screen width
#define	SCREEN_Y HIGH_RES_Y // screen height

#define MAX_OT_ENTRIES 4096

#ifndef SAFE_FREE
#define SAFE_FREE(x) { if ( (x) ) free( (x) ); (x) = 0; }
#endif // SAFE_FREE

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// --------------------
// STRUCTURES AND ENUMS
// --------------------

// a simple scratch allocator
typedef struct
{
    u_char *start, *end, *next;
}SCRATCH;

typedef struct
{
    SCRATCH scratch[2];
}DB_SCRATCH;

extern SCRATCH scratch_mem;
extern DB_SCRATCH db_scratch_mem;

void init_scratch(u_long inSize, SCRATCH* outScratchMem);
u_char* alloc_scratch(SCRATCH* inScratchMem, u_long inSize, u_short inAlignment);
void shutdown_scratch(SCRATCH* inScratchMem);

// Used as a mask to define vertex attribute availability
enum VTXATTR
{
    VTXATTR_POS = 1 << 0,
    VTXATTR_COLOR = 1 << 1,
    VTXATTR_TEXCOORD = 1 << 2,
    VTXATTR_GOURAUD = 1 << 3,
    VTXATTR_ALL = (VTXATTR_POS | VTXATTR_COLOR | VTXATTR_TEXCOORD),
    VTXATTR_ALL_GOURAUD = (VTXATTR_POS | VTXATTR_COLOR | VTXATTR_TEXCOORD | VTXATTR_GOURAUD)
};

// Used as a mask to define the active renderstate
enum RENDERSTATEFLAG
{
    RSF_FOG = 1 << 0
};

/*
Expected order of attributes are as follows:

 position:  0 [required]
 color:     1 [optional, white if not available]
 texcoord:  2 [optional, only used in _T poly variants]

Availability of attributes is defined using a mask as follows:
VTXATTR_POS
or
VTXATTR_POS | VTXATTR_COLOR
or
VTXATTR_POS | VTXATTR_TEXCOORD
etc, etc.

Position must always be present for the renderable instance to be considered.
*/

// Vertexbuffer
enum VBFLAG
{
    VB_DYNAMIC = 1 << 0
};

typedef struct
{
    void*       primmem;
    SVECTOR*    posmem;
    u_short     num_vertices;
    u_short     stride;
    u_short     attributes;
    u_short     primSize;
    u_char      usage;
    u_char      primIdx;
}VERTEXBUFFER;

void alloc_vb(VERTEXBUFFER* outVB, u_short inNumVertices, u_short inStride, u_short inAttributes, u_char inUsage);
void update_vb(VERTEXBUFFER* ioVB, void* inVertices);
void free_vb(VERTEXBUFFER* inVB);

// vertex structures
typedef struct
{
    SVECTOR position;
    CVECTOR color;
}V_PC;

typedef struct
{
    SVECTOR position;
    CVECTOR color;
    u_char u0, v0;
}V_PCT;

typedef struct
{
  SVECTOR position;
}V_P;

// Defines a series of transformations
typedef struct
{
    VECTOR position;
    SVECTOR rotation;
    SVECTOR scale;
}TRANSFORM;

typedef struct
{
    u_short	px,py;	// Image offset in the framebuffer
	u_short pw,ph;	// Image size
	u_short tpage;	// Framebuffer page ID of texture
	u_short mode;	// Color mode (0 - 4-bit, 1 - 8-bit, 2 - 16-bit, 3 - 24-bit)
	u_short cx,cy;	// Image CLUT offset in the framebuffer
	u_short clutid;	// TIM's CLUT ID
}TEXTURE;

// Defines something that can be added to the OT
typedef struct
{
    TRANSFORM*  transform;
    VERTEXBUFFER* vb;
    TEXTURE* tex;
}RENDERABLE;

// Defines a frame's buffer resource
typedef struct
{
    DRAWENV draw;
    DISPENV disp;
    u_long ot[MAX_OT_ENTRIES];
}DB;

// Defines camera transformation
typedef struct
{
    MATRIX viewTranslationMat;
    MATRIX viewRotationMat;
}CAMERA_MATRICES;

// Defines a simple render state
typedef struct
{
    u_short      flags;
}RENDER_STATE;

// ----------
// GLOBALS
// ----------
typedef struct
{
    DB db[2];
    DB* cdb;

    CAMERA_MATRICES camMatrices;
    CVECTOR clearColor;
    u_long frameIdx;
}GLOBALS;

extern GLOBALS *globals;
extern RENDER_STATE *rs;

void load_tim(u_long *inAddress, TEXTURE *outTexture);
void init_transform(TRANSFORM* ioTrans);
u_long align_size(u_long inSize, u_short inAlignment);
void update_camera(VECTOR* position, SVECTOR* rotation);
void init_system(int x, int y, int level, unsigned long stack, unsigned long heap);
void add_renderable(u_long* inOT, RENDERABLE* inRenderable);

#define INIT_MAIN_SCRATCH(size) init_scratch((size), &scratch_mem);
#define ALLOC_MAIN_SCRATCH(size, alignment) alloc_scratch(&scratch_mem, (size), (alignment));
#define RESET_MAIN_SCRATCH() scratch_mem.next = scratch_mem.start;
#define SHUTDOWN_MAIN_SCRATCH() shutdown_scratch(&scratch_mem);

#define INIT_MAIN_DB_SCRATCH(size) \
    init_scratch((size/2), &db_scratch_mem.scratch[0]); \
    init_scratch((size/2), &db_scratch_mem.scratch[1]);

#define ALLOC_MAIN_DB_SCRATCH(size, alignment) alloc_scratch(&db_scratch_mem.scratch[globals->frameIdx & 1], (size), (alignment));
#define RESET_MAIN_DB_SCRATCH() db_scratch_mem.scratch[globals->frameIdx & 1].next = db_scratch_mem.scratch[globals->frameIdx & 1].start;

#define SHUTDOWN_MAIN_DB_SCRATCH() \
    shutdown_scratch(&db_scratch_mem.scratch[0]); \
    shutdown_scratch(&db_scratch_mem.scratch[1]);


// allocations
extern u_long mem_allocated;

void* _calloc(size_t n, size_t s);
void* _malloc(size_t n);
void  _free(void*);

#endif /* ndef ____HELPER_H____ */
