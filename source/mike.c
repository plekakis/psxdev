/* mike.c */
#include "helper.h"

static SVECTOR x[3];
static VECTOR cameraPos = {0,0,1000};
static SVECTOR cameraRot = {0,0,100};
static TRANSFORM t;
static RENDERABLE triangle;

void mikeStart() {
	
	setVector(&x[0], -55, 12, 0);
    setVector(&x[1],  25, 18, 0);
    setVector(&x[2], 0,  -128, 0);


    t.position.vx = 0;
    t.position.vy = 0;
    t.position.vz = 0;

    t.rotation.vx = 0;
    t.rotation.vy = 0;
    t.rotation.vz = 0;

	init_renderable(&triangle, &t, 1, x, sizeof(SVECTOR));
}

void mikeUpdate() {
	/* clear all OT entries */
	ClearOTag(globals->cdb->ot, MAX_OT_ENTRIES);

	update_camera(&cameraPos, &cameraRot);
	add_renderable(globals->cdb->ot, &triangle);
}