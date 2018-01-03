/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	header for mesh handler
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

typedef struct {
	SVECTOR	x3;		/* 3D point */
	SVECTOR	x2;		/* 2D point (after perspective trans.) */
	/*int	sz;		/* otz */
} MESH;

void mesh_init(MESH *mp, int mx, int my, int width, int height);
void mesh_RotTransPers(MESH *mp, int mx, int my);
void mesh_AddPrim(u_long *ot, int otsize, POLY_FT4 *p, MESH *mp, int mx, int my);
void mesh_morf(MESH *mp, int mx, int my, int cx, int cy, int h);
void mesh_scroll(MESH *mp, int mx, int my, int rx, int ry);
