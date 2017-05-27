#include "helper.h"

GLOBALS *globals;
RENDER_STATE *rs;

#include "primptrs.c"

static GLOBALS _globals;
static RENDER_STATE _rs;

u_long align_size(u_long inSize, u_short inAlignment)
{
    return (inSize + (inAlignment - 1)) & ~(inAlignment - 1);
}

SCRATCH *scratch_mem;
static SCRATCH _scratch_mem;

void init_scratch(u_long inSize)
{
    scratch_mem = &_scratch_mem;

    scratch_mem->start = (u_char*)realloc3(scratch_mem->start, inSize);
	scratch_mem->end = ((u_char*)scratch_mem->start) + inSize;
	scratch_mem->next = scratch_mem->start;
}

u_char* alloc_scratch(u_long inSize, u_short inAlignment)
{
    u_char *m = 0;
    inSize = align_size(inSize, inAlignment);

    m = scratch_mem->next + inSize;

    if (m > scratch_mem->end) return 0;

    scratch_mem->next = m;
    return m;
}

void shutdown_scratch()
{
    free3(scratch_mem->start);

    scratch_mem->start = scratch_mem->end = scratch_mem->next = 0;
}

void update_camera(VECTOR* position, SVECTOR* rotation)
{
    MATRIX* transMat;

    TransMatrix(&globals->camMatrices.viewTranslationMat, position);
    transMat = &globals->camMatrices.viewTranslationMat;

    transMat->t[0] *= -1;
    transMat->t[1] *= -1;
    transMat->t[2] *= -1;

    RotMatrix(rotation, &globals->camMatrices.viewRotationMat);
    TransposeMatrix(&globals->camMatrices.viewRotationMat, &globals->camMatrices.viewRotationMat);
}

void init_system(int x, int y, int z, int level, unsigned long stack, unsigned long heap)
{
    u_short idx;

	globals = &_globals;
	rs = &_rs;

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
	SetDefDrawEnv(&globals->db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&globals->db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&globals->db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&globals->db[1].disp, 0,   0, 320, 240);

	for (idx=0; idx<MAX_BUFFERS; idx++)
    {
        globals->db[idx].draw.isbg = 1;
        setRGB0(&globals->db[idx].draw, 0, 64, 127);
    }

    SetDispMask(1);

    // setup initial renderstate
    // flat shading, no fog
    rs->flags = 0;

    // initialise function pointers for primitives
    fncInitPrimitive[PRIMIDX_G3] = init_prim_g3;
    fncInitPrimitive[PRIMIDX_F3] = init_prim_f3;
    fncInitPrimitive[PRIMIDX_GT3] = init_prim_gt3;
    fncInitPrimitive[PRIMIDX_FT3] = init_prim_ft3;

    fncPrimSetColors[PRIMIDX_G3] = prim_set_colors_g3;
    fncPrimSetColors[PRIMIDX_F3] = prim_set_colors_f3;
    fncPrimSetColors[PRIMIDX_GT3] = prim_set_colors_gt3;
    fncPrimSetColors[PRIMIDX_FT3] = prim_set_colors_ft3;
}

inline u_short get_prim_size(u_short inAttributes)
{
    u_short size = 0;

    if (rs->flags & RSF_GOURAUD)
    {
        if (inAttributes & VTXATTR_TEXCOORD)
        {
            size = sizeof(POLY_GT3);
        }
        else
        {
            size = sizeof(POLY_G3);
        }
    }
    else
    {
        if (inAttributes & VTXATTR_TEXCOORD)
        {
            size = sizeof(POLY_FT3);
        }
        else
        {
            size = sizeof(POLY_F3);
        }
    }

    return size;
}

// TODO: handle doublebuffered renderables
void add_renderable(u_long* inOT, RENDERABLE* inRenderable)
{
    if (inRenderable && (inRenderable->attributes & VTXATTR_POS))
    {
        u_short primIdx;
        u_char vertexIdx;
        u_short next_offset;
        long otZ;
        u_char fncIdx;
        u_short numPrims;
        u_short primSize;
        void* mem;
        MATRIX modelRotation, modelTranslation;

        u_int v_idx = 0;
        TRANSFORM* transform = inRenderable->transform;
        void* vertices = inRenderable->vertices;

        /* set rotation*/
        RotMatrix(&transform->rotation, &modelRotation);
        SetRotMatrix(MulMatrix2(&modelRotation, &globals->camMatrices.viewRotationMat));

        /* set translation*/
		TransMatrix(&modelTranslation, &transform->position);

		//SetTransMatrix(MulMatrix2(&globals->camMatrices.viewTranslationMat, &modelTranslation));
		SetTransMatrix(&globals->camMatrices.viewTranslationMat);

		numPrims = inRenderable->num_vertices / 3;
		primSize = get_prim_size(inRenderable->attributes);

        mem = alloc_scratch(primSize * numPrims, 16);

        // primitive classification
        if (primSize == sizeof(POLY_G3)) fncIdx = 0;
        else if (primSize == sizeof(POLY_F3)) fncIdx = 1;
        else if (primSize == sizeof(POLY_GT3)) fncIdx = 2;
        else if (primSize == sizeof(POLY_FT3)) fncIdx = 3;

        for (primIdx=0; primIdx<numPrims; ++primIdx)
        {
            void* xstart[3];
            void* primmem = (void*)((u_char*)mem + primIdx * primSize);

            // primitive classification
            (*fncInitPrimitive[fncIdx])(primmem, &xstart[0]);

            // position always comes first
            otZ = 0;
            for (vertexIdx=0; vertexIdx<3; ++vertexIdx)
            {
                long dummy0, dummy1;
                otZ += RotTransPers((SVECTOR*)(vertices + inRenderable->stride * vertexIdx), (long*)xstart[vertexIdx], &dummy0, &dummy1);
            }

            next_offset = sizeof(SVECTOR);

            // then color, if available
            // if it is not available, set to white
            {
                CVECTOR* color[3] = { 0xff, 0xff, 0xff };

                if (inRenderable->attributes & VTXATTR_COLOR)
                {
                    for (vertexIdx=0; vertexIdx<3; ++vertexIdx)
                    {
                        color[vertexIdx] = (CVECTOR*)((u_char*)vertices + inRenderable->stride * vertexIdx + next_offset);
                    }

                    next_offset += sizeof(CVECTOR);
                }

                (*fncPrimSetColors[fncIdx])(primmem, color);
            }

            if (inRenderable->attributes & VTXATTR_TEXCOORD)
            {
                next_offset += sizeof(SVECTOR);
            }

            AddPrim(inOT, primmem);

            v_idx += inRenderable->stride * 3;
            vertices = (u_char*)vertices + inRenderable->stride;
        }
    }
}
