/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *	2D effects
 */

#include "mesh.h"

#define MASK_X	8
#define MASK_Y	8

static int mask[MASK_Y][MASK_X] = {
        -1743, -689,  508, 1290, 1290,  508, -689,-1743,
         -689, 1290, 3291, 4541, 4541, 3291, 1290, -689,
          508, 3291, 5978, 7623, 7623, 5978, 3291,  508,
         1290, 4541, 7623, 9494, 9494, 7623, 4541, 1290,
         1290, 4541, 7623, 9494, 9494, 7623, 4541, 1290,
          508, 3291, 5978, 7623, 7623, 5978, 3291,  508,
         -689, 1290, 3291, 4541, 4541, 3291, 1290, -689,
        -1743, -689,  508, 1290, 1290,  508, -689,-1743,
};

void mesh_morf(MESH *mp, int mx, int my, int cx, int cy, int h)
{
	int	x, y;
	int	dmx, dmy;
	
	for (y = 0; y < my; y++) {
		for (x = 0; x < mx; x++, mp++) {
			
			if ((dmx = x-cx+MASK_X/2) < 0 || dmx >= MASK_X)
				continue;
			else if ((dmy = y-cy+MASK_Y/2) < 0 || dmy >= MASK_Y)
				continue;
			mp->x3.vz = mask[dmy][dmx]*h>>14;
		}
	}
}

void mesh_scroll(MESH *mp, int mx, int my, int rx, int ry)
{
	int		i, x, y;
	int		sx, sy;
	int		dsx = 4096/mx, dsy = 4096/my;
	int		dx, dy;
	
	for (sy = y = 0; y < my; y++, sy += dsy) {
		dx = rx*rsin(sy)>>12;
		for (sx = x = 0; x < mx; x++, sx += dsx, mp++) {
			dy = ry*rsin(sx)>>12;
			mp->x3.vx += dx;
			mp->x3.vy += dy;
		}
	}
}
