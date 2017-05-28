/* alkis.c */
#include "helper.h"
#include "app.h"

#ifndef USE_MIKE_CODE

static VECTOR cameraPos = {0,0,0};
static SVECTOR cameraRot = {0,0,0};

static TRANSFORM t;
static RENDERABLE triangle;

static TRANSFORM dynamicT;
static RENDERABLE dynamicTri;

static VERTEXBUFFER triVB;
static VERTEXBUFFER dynamicTriVB;


void start() {

    V_PC tri_verts[6];

	setVector(&tri_verts[0].position, -320, 128, 0);
    setVector(&tri_verts[1].position, -128, 128, 0);
    setVector(&tri_verts[2].position, -128, -128, 0);

    setVector(&tri_verts[3].position, 0, 128, 0);
    setVector(&tri_verts[4].position, 256, 128, 0);
    setVector(&tri_verts[5].position, 256,  -128, 0);

    setColor(&tri_verts[0].color, 255, 0, 0);
    setColor(&tri_verts[1].color, 0, 255, 0);
    setColor(&tri_verts[2].color, 0, 0, 255);

    setColor(&tri_verts[3].color, 255, 0, 0);
    setColor(&tri_verts[4].color, 0, 255, 0);
    setColor(&tri_verts[5].color, 0, 0, 255);

    alloc_vb(&triVB, 6, sizeof(V_PC), VTXATTR_POS | VTXATTR_COLOR | VTXATTR_GOURAUD, 0);
    update_vb(&triVB, tri_verts);

    init_transform(&t);
    t.position.vy = -150;
    t.position.vz = 500;

    triangle.transform = &t;
    triangle.vb = &triVB;

    // vb for dynamic triangle
    alloc_vb(&dynamicTriVB, 3, sizeof(V_PC), VTXATTR_POS | VTXATTR_COLOR | VTXATTR_GOURAUD, VB_DYNAMIC);
}

float xx = 0;
CVECTOR dynRGB = {0x0, 0x0, 0x0};

void update() {
    V_PC v[3];

	update_camera(&cameraPos, &cameraRot);

    SetFarColor(0, 0, 0);

	/* start point of depth quweue*/
	SetFogNearFar(350, 500, SCREEN_X);

	xx = sin(globals->frameIdx * 0.25f) * 200.0f;

    dynRGB.r = MIN(255, 75 + (sin(globals->frameIdx * 0.95f + PI/4) * 0.5f + 0.5f) * (255-75));
    dynRGB.g = MIN(255, 60 + (sin(globals->frameIdx * 0.7f + PI/2) * 0.5f + 0.5f) * (255-60));
    dynRGB.b = MIN(255, 150 + (sin(globals->frameIdx * 1.2f - PI) * 0.5f + 0.5f) * (255-150));

	setVector(&v[0].position, 0 + xx/2, -128, 0);
    setVector(&v[1].position, 128 + xx, 128, 0);
    setVector(&v[2].position, -128 + xx, 128, 0);

    setColor(&v[0].color, dynRGB.r, 0, 0);
    setColor(&v[1].color, 0, dynRGB.g, 0);
    setColor(&v[2].color, 0, 0, dynRGB.b);

    update_vb(&dynamicTriVB, v);

    init_transform(&dynamicT);
    dynamicT.position.vy = 200;
    dynamicT.position.vz = 500;

    dynamicTri.transform = &dynamicT;
    dynamicTri.vb = &dynamicTriVB;

	add_renderable(globals->cdb->ot, &triangle);
	add_renderable(globals->cdb->ot, &dynamicTri);
}

void shutdown() {
    free_vb(&triVB);
    free_vb(&dynamicTriVB);
}
#endif
