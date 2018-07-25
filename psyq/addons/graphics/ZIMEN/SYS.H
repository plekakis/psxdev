/* $PSLibId: Runtime Library Release 3.6$ */
/*				divproto.h
 *
 *		Copyright (C) 1993/1994 by Sony Corporation
 *			All rights Reserved
 *
 */
#include "mesh.h"

/*
 * Screen basic information
 */
#define	SCR_X	(640/2)				/* screen width */
#define	SCR_Y	(480/2)				/* screen width */
#define SCR_Z	(SCR_W)			/* distance to screen */
#define SCR_W	(SCR_X*2)
#define SCR_H	(SCR_Y*2)

/*
 * Geometry Attributes;
 */
typedef struct {
	SVECTOR	dvs;			/* moving direction (in the screen) */
	SVECTOR	angle;			/* local-screen angle */
	VECTOR	home;			/* current home position */
	short	rx, ry;			/* wrap-round unit */
	short	fog_far, fog_near;	/* fog parameters */
	MATRIX	mat;			/* local-screen matrix */
} GEOMENV;

/*
 * get min/max
 */
#define max(x,y)	((x)>(y)?(x):(y))
#define max3(x,y,z)	((x)>(y)?max(x,z):max(y,z))
#define max4(x,y,z,w)	(max(x,y)>max(z,w)?max(x,y):max(z,w))

#define min(x,y)	((x)<(y)?(x):(y))
#define min3(x,y,z)	((x)<(y)?min(x,z):min(y,z))
#define min4(x,y,z,w)	(min(x,y)<min(z,w)?min(x,y):min(z,w))

/*
 * prototypes
 */
void divPolyClip(RECT *rect, int t, int n);
POLY_FT4 *divPolyFT4(u_long *ot, SVECTOR *x0, SVECTOR *x1, SVECTOR *x2,
		     SVECTOR *x3, POLY_FT4 *tmpl, POLY_FT4 *heap,
		     u_long *scratch);

void disp_clipw(RECT32 *rect);
void disp_clips(RECT *rect);
void draw_line4w(u_char col,
		     SVECTOR *x0, SVECTOR *x1, SVECTOR *x2, SVECTOR *x3);
void draw_line4s(u_char col,
		     SVECTOR *x0, SVECTOR *x1, SVECTOR *x2, SVECTOR *x3);


int  padRead(GEOMENV *genv);
void geomUpdate(GEOMENV *genv);
int areaClipZ(RECT *clips, RECT32 *clipw, int limit);
void setMeshConfig(MESH *mesh, GEOMENV *genv);
void initMap(MESH *mesh);





