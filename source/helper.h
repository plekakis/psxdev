
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
#define ORIGIN_X  160 // screen width
#define	ORIGIN_Y 120 // screen height

#define MAX_OT_ENTRIES 1
#define MAX_G3 1

#define MAX_BUFFERS 2

#ifndef SAFE_FREE
#define SAFE_FREE(x) { if ( (x) ) free( (x) ); (x) = 0; }
#endif // SAFE_FREE

// --------------------
// STRUCTURES AND ENUMS
// --------------------

// a simple scratch allocator
typedef struct
{
    u_char *start, *end, *next;
}SCRATCH;

extern SCRATCH *scratch_mem;
void init_scratch(u_long inSize);
u_char* alloc_scratch(u_long inSize, u_short inAlignment);
void shutdown_scratch();

// Used as a mask to define vertex attribute availability
enum VTXATTR
{
    VTXATTR_POS = 1 << 0,
    VTXATTR_COLOR = 1 << 1,
    VTXATTR_TEXCOORD = 1 << 2,
    VTXATTR_ALL = (VTXATTR_POS | VTXATTR_COLOR | VTXATTR_TEXCOORD)
};

// Used as a mask to define the active renderstate
enum RENDERSTATEFLAG
{
    RSF_FOG = 1 << 0,
    RSF_GOURAUD = 1 << 1
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

// vertex structures
typedef struct
{
    SVECTOR position;
    CVECTOR color;
}V_PC;

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

// Defines something that can be added to the OT
typedef struct
{
    TRANSFORM*  transform;
    void*       vertices;
    u_short     num_vertices;
    u_short     stride;
    u_short     attributes;
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
    DB db[MAX_BUFFERS];
    DB* cdb;

    CAMERA_MATRICES camMatrices;

    u_long frameIdx;
}GLOBALS;

extern GLOBALS *globals;
extern RENDER_STATE *rs;

u_long align_size(u_long inSize, u_short inAlignment);
void update_camera(VECTOR* position, SVECTOR* rotation);
void init_system(int x, int y, int z, int level, unsigned long stack, unsigned long heap);
void add_renderable(u_long* inOT, RENDERABLE* inRenderable);

#endif /* ndef ____HELPER_H____ */
