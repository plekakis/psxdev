#include "helper.h"

GLOBALS *globals;
RENDER_STATE *rs;

#include "primptrs.c"
#include "memalloc.c"
#define VERTEXCOUNT_PER_PRIM 3

static GLOBALS _globals;
static RENDER_STATE _rs;

void init_transform(TRANSFORM* ioTrans)
{
    setVector(&ioTrans->position, 0, 0, 0);
    setVector(&ioTrans->rotation, 0, 0, 0);
    setVector(&ioTrans->scale, 1, 1, 1);
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

void init_system(int x, int y, int level, unsigned long stack, unsigned long heap)
{
    u_short idx;
    const u_short centerx = x/2;
    const u_short centery = y/2;

	globals = &_globals;
	rs = &_rs;

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
	SetGeomOffset(centerx, centery);

	/* distance to veiwing-screen*/
	SetGeomScreen(centerx);

    if (SCREEN_X == LOW_RES_X)
    {
        /* define frame double buffer */
        /*	buffer #0:	(0,  0)-(320,240) (320x240)
         *	buffer #1:	(0,240)-(320,480) (320x240)
        */
        SetDefDrawEnv(&globals->db[0].draw, 0,   0, SCREEN_X, SCREEN_Y);
        SetDefDrawEnv(&globals->db[1].draw, 0, SCREEN_Y, SCREEN_X, SCREEN_Y);
        SetDefDispEnv(&globals->db[0].disp, 0, SCREEN_Y, SCREEN_X, SCREEN_Y);
        SetDefDispEnv(&globals->db[1].disp, 0,   0, SCREEN_X, SCREEN_Y);
    }
    else
    {
        SetDefDrawEnv(&globals->db[0].draw, 0,   0, SCREEN_X, SCREEN_Y);
        SetDefDrawEnv(&globals->db[1].draw, 0,   0, SCREEN_X, SCREEN_Y);
        SetDefDispEnv(&globals->db[0].disp, 0,   0, SCREEN_X, SCREEN_Y);
        SetDefDispEnv(&globals->db[1].disp, 0,   0, SCREEN_X, SCREEN_Y);
    }

    SetDispMask(1);

    // setup initial renderstate
    rs->flags = 0;

    // initial clear color
    setColor(&globals->clearColor, 32, 127, 255);

    globals->frameIdx = 0;

    // initialise function pointers for primitives
    fncInitPrimitive[PRIMIDX_G3] = &init_prim_g3;
    fncInitPrimitive[PRIMIDX_F3] = &init_prim_f3;
    fncInitPrimitive[PRIMIDX_GT3] = &init_prim_gt3;
    fncInitPrimitive[PRIMIDX_FT3] = &init_prim_ft3;

    fncPrimSetColors[PRIMIDX_G3] = &prim_set_colors_g3;
    fncPrimSetColors[PRIMIDX_F3] = &prim_set_colors_f3;
    fncPrimSetColors[PRIMIDX_GT3] = &prim_set_colors_gt3;
    fncPrimSetColors[PRIMIDX_FT3] = &prim_set_colors_ft3;

    fncPrimSetUVs[PRIMIDX_GT3] = &prim_set_uvs_gt3;
    fncPrimSetUVs[PRIMIDX_FT3] = &prim_set_uvs_ft3;

    fncPrimSetTPageClut[PRIMIDX_GT3] = &prim_set_tpageclut_gt3;
    fncPrimSetTPageClut[PRIMIDX_FT3] = &prim_set_tpageclut_ft3;
}

inline u_short get_prim_size(u_short inAttributes)
{
    u_short size = 0;

    if (inAttributes & VTXATTR_GOURAUD)
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

u_char get_prim_idx(u_short inPrimSize)
{
    u_char idx;

    if (inPrimSize == sizeof(POLY_G3)) idx = PRIMIDX_G3;
    else if (inPrimSize == sizeof(POLY_F3)) idx = PRIMIDX_F3;
    else if (inPrimSize == sizeof(POLY_GT3)) idx = PRIMIDX_GT3;
    else if (inPrimSize == sizeof(POLY_FT3)) idx = PRIMIDX_FT3;

    return idx;
}

//
// VERTEXBUFFER
//
void alloc_vb(VERTEXBUFFER* outVB, u_short inNumVertices, u_short inStride, u_short inAttributes, u_char inUsage)
{
    outVB->attributes = inAttributes;
    outVB->num_vertices = inNumVertices;
    outVB->stride = inStride;
    outVB->usage = inUsage;

    outVB->primSize = get_prim_size(outVB->attributes);
    outVB->primIdx = get_prim_idx(outVB->primSize);

    // allocate memory at this point if the vb is not dynamic
    // dynamic vbs use the double buffered scratch allocator
    if ((inUsage & VB_DYNAMIC) == 0)
    {
        outVB->primmem = (void*)calloc3((inNumVertices * outVB->primSize) / VERTEXCOUNT_PER_PRIM);
        outVB->posmem = (SVECTOR*)calloc3(inNumVertices * sizeof(SVECTOR));
    }
}

void update_vb(VERTEXBUFFER* ioVB, void* inVertices)
{
    u_short numPrims = ioVB->num_vertices / VERTEXCOUNT_PER_PRIM;
    const u_char isDynamic = (ioVB->usage & VB_DYNAMIC) != 0;
    void* vertices = inVertices;
    u_short primIdx;
    u_char vertexIdx;
    u_short next_offset;
    u_short posIdx = 0;

    // allocate memory for dynamic buffers
    if (isDynamic)
    {
        ioVB->primmem = ALLOC_MAIN_DB_SCRATCH((ioVB->num_vertices * ioVB->primSize) / VERTEXCOUNT_PER_PRIM, 16);
        ioVB->posmem = (SVECTOR*)ALLOC_MAIN_DB_SCRATCH(ioVB->num_vertices * sizeof(SVECTOR), 16);
    }

    for (primIdx=0; primIdx<numPrims; ++primIdx)
    {
        void* primmem = (void*)((u_char*)ioVB->primmem + primIdx * ioVB->primSize);

        for (vertexIdx=0; vertexIdx<VERTEXCOUNT_PER_PRIM; ++vertexIdx)
        {
            if (isDynamic)
            {
                const SVECTOR* vv = (const SVECTOR*)((u_char*)vertices + ioVB->stride * vertexIdx);
                setVector(&ioVB->posmem[posIdx], vv->vx, vv->vy, vv->vz);
            }
            else
            {
                memcpy(&ioVB->posmem[posIdx], (void*)((u_char*)vertices + ioVB->stride * vertexIdx), sizeof(SVECTOR));
            }

            ++posIdx;
        }

        next_offset = sizeof(SVECTOR);

        // then color, if available
        // if it is not available, set to white
        {
            CVECTOR* color[VERTEXCOUNT_PER_PRIM];
            if (ioVB->attributes & VTXATTR_COLOR)
            {
                for (vertexIdx=0; vertexIdx<VERTEXCOUNT_PER_PRIM; ++vertexIdx)
                {
                    color[vertexIdx] = (CVECTOR*)((u_char*)vertices + ioVB->stride * vertexIdx + next_offset);
                }

                next_offset += sizeof(CVECTOR);
            }

            (*fncPrimSetColors[ioVB->primIdx])(primmem, color);
        }

        if (ioVB->attributes & VTXATTR_TEXCOORD)
        {
            u_char* uvs;
            u_char us[VERTEXCOUNT_PER_PRIM];
            u_char vs[VERTEXCOUNT_PER_PRIM];

            for (vertexIdx=0; vertexIdx<VERTEXCOUNT_PER_PRIM; ++vertexIdx)
            {
                uvs = (u_char*)vertices + ioVB->stride * vertexIdx + next_offset;

                us[vertexIdx] = uvs[0];
                vs[vertexIdx] = uvs[1];
            }

            (*fncPrimSetUVs[ioVB->primIdx])(primmem, us, vs);
        }

        // advance
        vertices = (void*)((u_char*)vertices + ioVB->stride * VERTEXCOUNT_PER_PRIM);
    }
}

void free_vb(VERTEXBUFFER* inVB)
{
    if ((inVB->usage & VB_DYNAMIC) == 0)
    {
        if (inVB->primmem) free3(inVB->primmem);
        if (inVB->posmem) free3(inVB->posmem);
    }

    inVB->primmem = 0;
    inVB->posmem = 0;
}

// TODO: handle doublebuffered renderables
void add_renderable(u_long* inOT, RENDERABLE* inRenderable)
{
    if (inRenderable && (inRenderable->vb->attributes & VTXATTR_POS))
    {
        u_short primIdx;
        u_short posIdx = 0;
        u_char vertexIdx;
        VERTEXBUFFER* vb = inRenderable->vb;
        const u_short numPrims = vb->num_vertices / VERTEXCOUNT_PER_PRIM;
        const u_short tpage = inRenderable->tex ? inRenderable->tex->tpage : 0;
        const u_short clut = inRenderable->tex ? inRenderable->tex->clutid : 0;
        MATRIX modelRotation, modelTranslation;
        TRANSFORM* transform = inRenderable->transform;

        PushMatrix();

        RotMatrix(&transform->rotation, &modelRotation);
        SetRotMatrix(MulMatrix2(&modelRotation, &globals->camMatrices.viewRotationMat));

		TransMatrix(&modelTranslation, &transform->position);

		SetTransMatrix(&modelTranslation);
		//SetTransMatrix(MulMatrix2(&globals->camMatrices.viewTranslationMat, &modelTranslation));
		//SetTransMatrix(&globals->camMatrices.viewTranslationMat);


        for (primIdx=0; primIdx<numPrims; ++primIdx)
        {
            void* primmem = (void*)((u_char*)vb->primmem + primIdx * vb->primSize);
            long* xstart[3];
            int isomote;
            long otz, p, flg;

            (*fncInitPrimitive[vb->primIdx])(primmem, (void*)&xstart[0]);

            // position always comes first
            for (vertexIdx=0; vertexIdx<3; ++vertexIdx)
            {
                long dummy0, dummy1;
                RotTransPers(&vb->posmem[posIdx++], (long*)xstart[vertexIdx], &dummy0, &dummy1);
            }

            /* Translate from local coordinates to screen coordinates
            *  using RotAverageNclip4().
            *  otz represents 1/4 value of the average of z value of
            *  each vertex.
            */
            isomote = RotAverageNclip3(&vb->posmem[0], &vb->posmem[1], &vb->posmem[2],
                                       (long *)&xstart[0], (long *)&xstart[1], (long *)&xstart[2], &p, &otz, &flg);

            if (isomote <= 0) continue;

            // update clut and tpage
            if (inRenderable->vb->attributes & VTXATTR_TEXCOORD)
            {
                (*fncPrimSetTPageClut[inRenderable->vb->primIdx])(primmem, clut, tpage);
            }

            // handle fog
            if (rs->flags & RSF_FOG)
            {
                // update the color

            }
            AddPrim(inOT + otz, primmem);
        }

        PopMatrix();
    }
}

void load_tim(u_long *inAddress, TEXTURE *outTexture) {
    /*
		0: 4-bit CLUT
		1: 8-bit CLUT
		2: 16-bit DIRECT
		3: 24-bit DIRECT
	*/

	TIM_IMAGE timinfo;

	// Get the TIM's header
	if (OpenTIM(inAddress) == 0) {

        ReadTIM(&timinfo);

        // Upload the TIM image data to the framebuffer
        LoadImage(timinfo.prect, timinfo.paddr);
        DrawSync(0);

        // Set parameters for outTexture
        outTexture->px = timinfo.prect->x;
        outTexture->py = timinfo.prect->y;
        outTexture->pw = timinfo.prect->w;
        outTexture->ph = timinfo.prect->h;
        outTexture->tpage = GetTPage(timinfo.mode, 0, timinfo.prect->x, timinfo.prect->y);

        // If TIM has a CLUT (if color depth is lower than 16-bit), upload it as well
        if ((timinfo.mode & 3) < 2) {
            LoadImage(timinfo.crect, timinfo.caddr);
            DrawSync(0);

            outTexture->cx = timinfo.crect->x;
            outTexture->cy = timinfo.crect->y;
            outTexture->clutid = GetClut(timinfo.crect->x, timinfo.crect->y);
        }
	}
}
