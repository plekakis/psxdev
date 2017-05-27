/* alkis.c */
#include "helper.h"
#include "app.h"

#ifndef USE_MIKE_CODE

static V_PC tri_verts[6];
static VECTOR cameraPos = {0,0,-1000};
static SVECTOR cameraRot = {0,0,100};
static TRANSFORM t;
static RENDERABLE triangle;

void start() {

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

    t.position.vx = 0;
    t.position.vy = 0;
    t.position.vz = 0;

    t.rotation.vx = 0;
    t.rotation.vy = 0;
    t.rotation.vz = 0;

    triangle.num_vertices = 6;
    triangle.vertices = tri_verts;
    triangle.stride = sizeof(V_PC);
    triangle.attributes = VTXATTR_POS | VTXATTR_COLOR;
    triangle.transform = &t;
}

void update() {
	update_camera(&cameraPos, &cameraRot);

    rs->flags = RSF_GOURAUD;
	add_renderable(globals->cdb->ot, &triangle);
}

void shutdown() {

}
#endif
