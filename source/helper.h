
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

#define MAX_BUFFERS 2

// ----------
// STRUCTURES
// ----------

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
    TRANSFORM* trans;
    POLY_G3* triangles;
    void* vertices;
    u_short num_triangles;
    u_short num_vertices;
    u_int stride;
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


extern void update_camera(VECTOR* position, SVECTOR* rotation);

extern void init_system(int x, int y, int z, int level, unsigned long stack, unsigned long heap);

extern void init_renderable(RENDERABLE* r, TRANSFORM* t, u_short count, void* vertices, u_short stride);

extern void destroy_renderable(RENDERABLE* r);

extern void add_renderable(u_long* ot, RENDERABLE* r);
