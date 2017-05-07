
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
    VECTOR rotation;
    VECTOR scale;
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
    VECTOR position;
    VECTOR look;
}CAMERA;

// ----------
// GLOBALS
// ----------
DB db[MAX_BUFFERS];
DB* cdb;

CAMERA cdebug;
CAMERA* ccam = &cdebug;

u_long frameIdx = 0;

void init_system(int x, int y, int z, int level, unsigned long stack, unsigned long heap)
{
    u_short idx;

    /* Initialise with 16kb of stack and 1mb of heap */
    InitHeap3((void*)stack, heap);

    ResetGraph(0);		/* initialize Renderer		*/

	/* reset graphic subsystem*/
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));

	/* set debug mode (0:off, 1:monitor, 2:dump)  */
	SetGraphDebug(level);

	/* initialize geometry subsystem*/
	InitGeom();

	CdInit();		/* initialize CD-ROM		*/

    PadInit(0);     /* initialize input */

	/* set geometry origin as (160, 120)*/
	SetGeomOffset(x, y);

	/* distance to veiwing-screen*/
	SetGeomScreen(z);

	/* define frame double buffer */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	for (idx=0; idx<MAX_BUFFERS; idx++)
    {
        db[idx].draw.isbg = 1;
        setRGB0(&db[idx].draw, 0, 64, 127);
    }

    SetDispMask(1);
}

void init_renderable(RENDERABLE* r, TRANSFORM* t, u_short count, void* vertices, u_short stride)
{
    u_short i;

    if (r)
    {
        (*r).trans = t;

        (*r).triangles = (POLY_G3*)malloc3(sizeof(POLY_G3) * count);
        (*r).num_triangles = count;

        (*r).num_vertices = (*r).num_triangles * 3;
        (*r).stride = stride;
        memcpy(&(*r).vertices, &vertices, (*r).num_vertices * sizeof(stride));

        for (i=0; i<count; ++i)
        {
            SetPolyG3(&(*r).triangles[i]);

            /* set colors for each vertex*/
            setRGB0(&(*r).triangles[i], 0xff, 0x00, 0x00);
            setRGB1(&(*r).triangles[i], 0x00, 0xff, 0x00);
            setRGB2(&(*r).triangles[i], 0x00, 0x00, 0xff);
        }
    }
}

void destroy_renderable(RENDERABLE* r)
{
    if (r && (*r).triangles)
    {
        free((*r).triangles);
    }
}

void add_renderable(u_long* ot, RENDERABLE* r)
{
    u_short i;
    long dummy0, dummy1;
    u_int v_idx = 0;

    if (r)
    {
        const void * v = r->vertices;
        for (i=0; i<r->num_triangles; ++i)
        {
            RotTransPers((SVECTOR*)(v + r->stride * (v_idx + 0)), (long *)&r->triangles[i].x0, &dummy0, &dummy1);
            RotTransPers((SVECTOR*)(v + r->stride * (v_idx + 1)), (long *)&r->triangles[i].x1, &dummy0, &dummy1);
            RotTransPers((SVECTOR*)(v + r->stride * (v_idx + 2)), (long *)&r->triangles[i].x2, &dummy0, &dummy1);

            AddPrim(ot, &r->triangles[i]);

            v_idx += r->stride * 3;
        }
    }
}
