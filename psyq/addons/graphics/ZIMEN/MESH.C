/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *				mesh
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Nov,17,1993	suzu	
 *	2.00		Jan,17,1994	suzu	(rewrite)
 *	3.00		Jun,19,1995	suzu	(rewrite)
 *	3.10		Feb,15,1996	suzu	
 *
 *			    mesh handler
 :		       メッシュデータの取り扱い
 */
#include <sys/types.h>
#include "mesh.h"

typedef struct {
	SVECTOR	x3;			/* 3D point */
	SVECTOR	x2;			/* 2D point */
	long	p;			/* depth queue */
	u_short	clip;			/* is in clip area */
} MCELL;

static MCELL	mcell[MESH_NY*MESH_NX];

static void dump_clip(int nx, int ny);

POLY_FT4 *meshRotTransPers(u_long *ot, int otsize, MESH *mesh, POLY_FT4 *heap)
{
	MCELL	*cell, *cp0, *cp1, *cp2, *cp3;
	MAP	*map,  *mp0;
	
	long	x, y, z;			/* work */
	int	vx, vy;
	long	dmy;
	int	vsx, vsy;		/* left-upper coordinate */
	int	msx, msy;		/* left-upper corner of map */
	int	mex, mey;		/* right-lower corner of map */
	int	ssx, ssy, sex, sey;	/* clip window in the screen */
	
	/* set clip rectangle in the world coordinate 
	 * Note that clip value is quantized by cell size
	 * (mesh->ux, mesh->uy)
 	 */	 
	if (mesh->clipw) {
		int	wsx = mesh->clipw->x+mesh->ox;
		int	wsy = mesh->clipw->y+mesh->oy;
		
		/* right-upper corner of map */
		msx = wsx/mesh->ux;
		msy = wsy/mesh->uy;
		limitRange(msx, 0, mesh->mx-1);
		limitRange(msy, 0, mesh->my-1);
		
		/* width/height of mesh */
		mex = (wsx+mesh->clipw->w)/mesh->ux+1;
		mey = (wsy+mesh->clipw->h)/mesh->uy+1;
		limitRange(msx, 0, mesh->mx-1);
		limitRange(msy, 0, mesh->my-1);
		
		limitRange(mex, 0, msx+MESH_NX-1);
		limitRange(mey, 0, msy+MESH_NY-1);
		
		/* right-upper corner of mesh */
		vsx = msx*mesh->ux-mesh->ox;
		vsy = msy*mesh->uy-mesh->oy;
	}
	/* if clip window is not specified, use default */
	else {
		msx = msy = 0;
		mex = MESH_NX-1;
		mey = MESH_NX-1;
		vsx = -mesh->ox;
		vsy = -mesh->oy;
	}
	
	/* set clip rectangle in the screen coordinate */
	if (mesh->clips) {
		
		ReadGeomOffset(&x, &y);	/* add the current GTE offset */
		
		ssx = x+mesh->clips->x;
		ssy = y+mesh->clips->y;
		sex = x+mesh->clips->x+mesh->clips->w;
		sey = y+mesh->clips->y+mesh->clips->h;
	}
	/* if clip window is not specified, use default */
	else {
		ssx = ssy = 0;
		sex = 640; sey = 480;
	}
	
	/*
	 * RotTransPers.
	 * RotTransPers is calculated only in clipw rectangle area.
	 */
	vy   = vsy;				/* map start point */
	cell = mcell;

	for (y = msy; y < mey+1; y++, vy += mesh->uy, cell += MESH_NX) {
		map = mesh->map+(y&mesh->msk_y)*(mesh->msk_x+1);
		vx  = vsx;
		cp0 = cell;
		
		for (x = msx; x < mex+1; x++, vx += mesh->ux, cp0++) {
			
			mp0 = map+(x&mesh->msk_x);

			cp0->x3.vx = mp0->v.vx + vx;
			cp0->x3.vy = mp0->v.vy + vy;
			cp0->x3.vz = mp0->v.vz;

			cp0->x2.vz = RotTransPers(&cp0->x3,
					 (long *)&cp0->x2, &cp0->p, &dmy);

			/* If the translated point is out of range,
			 *  set clip flag.
			 */
			cp0->clip = 0;
			if (cp0->x2.vx > sex)	cp0->clip |= 0x01;
			if (cp0->x2.vx < ssx)	cp0->clip |= 0x02;
			if (cp0->x2.vy > sey)	cp0->clip |= 0x04;
			if (cp0->x2.vy < ssy)	cp0->clip |= 0x08;
		}
	}
	/* for debug */
	if (mesh->debug&0x02)
		dump_clip(mex-msx, mey-msy);
	
	/* add to OT */
	cell = mcell;
	for (y = msy; y < mey; y++, cell += MESH_NX) {
		map = mesh->map+(y&mesh->msk_y)*(mesh->msk_x+1);
		cp0 = cell;		cp1 = cp0+1;
		cp2 = cp0+MESH_NX;	cp3 = cp2+1;

		for (x = msx; x < mex;  x++, cp0++, cp1++, cp2++, cp3++) {
			
			/* update map */
			mp0 = map+(x&mesh->msk_x);
		
			/* if all 4 point is out of range, skip */
			if (cp0->clip&cp1->clip&cp2->clip&cp3->clip)
				continue;

			/* detect z */
			if ((z=(cp0->x2.vz+cp3->x2.vz)>>1) >= otsize || z < 0)
				continue;
			
			/* set XY */
			setXY4(heap,
			       cp0->x2.vx,cp0->x2.vy, cp1->x2.vx,cp1->x2.vy, 
			       cp2->x2.vx,cp2->x2.vy, cp3->x2.vx,cp3->x2.vy); 
			
			/* set color */
			DpqColor(&mp0->c, cp0->p, (CVECTOR *)&heap->r0);

			/* set ID */
			if (mesh->debug&0x01)	setPolyFT3(heap);
			else			setPolyFT4(heap);
			
			/* set texture */
			heap->tpage = mesh->tpage;
			heap->clut  = mesh->clut;
			setUVWH(heap, mp0->u0, mp0->v0, mp0->du, mp0->dv);

			/* if subdivision is required, call divPolyFT4 */
			if (z < mesh->divz) {
				heap = divPolyFT4(ot+z,
					  &cp0->x3, &cp1->x3,
					  &cp2->x3, &cp3->x3,
					  heap, heap, getScratchAddr(0));

			}
			else {
				addPrim(ot+z, heap);
				heap++;
			}
		}
	}
	return(heap);
}

/*
 *	Dump clipping status
 */	
static void dump_clip(int nx, int ny)
{
	static int id = -1;
	int	x, y;
	MCELL	*cp;
	char	buf[MESH_NX+2];
	if (id == -1) 
		id = FntOpen(500, 32, 0, 0, 2, MESH_NX*MESH_NY);

	FntPrint(id, "clip status\n");
	for (y = 0; y < ny; y++) {
		for (cp = mcell+y*MESH_NX, x = 0; x < nx; x++, cp++) 
			buf[x] = cp->clip? '.': '*';
		buf[x++] = '\0';
		FntPrint(id, "%s\n", buf);
	}
	FntFlush(id);
}

