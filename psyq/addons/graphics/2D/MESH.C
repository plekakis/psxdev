/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *			mesh: sample program
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Nov,17,1993	suzu	
 *	2.00		Jan,17,1994	suzu	(rewrite)
 *
 */
#include <sys/types.h>
#include "mesh.h"

void mesh_init(MESH *mp, int mx, int my, int width, int height)
{
	int	x, y, tx, ty;
	int	stx = -width/2;
	int	sty = -height/2;
	int	dtx = width/(mx-1);
	int	dty = height/(my-1);
	
	for (y = 0, ty = sty; y < my; y++, ty += dty) 
		for (x = 0, tx = stx; x < mx; x++, tx += dtx, mp++) {
			mp->x3.vx = tx;
			mp->x3.vy = ty;
			mp->x3.vz = 0;
		}
}	

void mesh_RotTransPers(MESH *mp, int mx, int my)
{
	int	x, y;
	long	dmy, flg;
	for (y = 0; y < my; y++)
		for (x = 0; x < mx; x++, mp++) 
			mp->x2.vz = RotTransPers(&mp->x3, 
						(long *)&mp->x2, &dmy, &flg);

}

void mesh_AddPrim(u_long *ot,
		  int otsize, POLY_FT4 *p, MESH *mp, int mx, int my)
{
	int	x, y, z;

	for (y = 0; y < my-1; y++) {
		for (x = 0; x < mx-1; x++, p++, mp++) {
			setXY4(p,
			       mp[   0].x2.vx, mp[   0].x2.vy,
			       mp[   1].x2.vx, mp[   1].x2.vy,
			       mp[  mx].x2.vx, mp[  mx].x2.vy,
			       mp[mx+1].x2.vx, mp[mx+1].x2.vy);

			z = otsize-mp->x2.vz;
			limitRange(z, 1, otsize-1);
			AddPrim(ot+z, p);
		}
		mp++;
	}
}	

