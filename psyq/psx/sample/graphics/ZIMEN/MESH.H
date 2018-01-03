/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *				mesh.h
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Nov,17,1993	suzu	
 *	2.00		Jan,17,1994	suzu	(rewrite)
 *
 *			    header for mesh */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Mesh array size */
#define MESH_NX	16
#define	MESH_NY	16

/*
 * Map pattern used in MESH */
typedef struct {
	SVECTOR	v;			/* distortion */
	CVECTOR	c;			/* color */
	u_char	u0, v0;			/* texture point */
	u_char	du, dv;			/* texture width/height */
	u_short	attr;			/* attribute */
	u_short	clip;			/* is in clip area */

} MAP;

/*
 * MESH structure
 * Map size (mx,my) have to be 2^n. */
typedef struct {
	MAP	*map;		/* map */
	RECT32	*clipw;		/* clip window (world) */
	RECT	*clips;		/* clip window (screen) */
	int	ux, uy;		/* cell unit size */
	int	mx, my;		/* map size */
	int	msk_x, msk_y;	/* map mask (should be 2^n-1) */
	int	ox, oy;		/* mesh offset */
	int	divz;		/* sub-dividison start point */
	int	ndiv;		/* max subdivision point */
	int	size;		/* threshold size */
	int	debug;		/* debug flag */
	u_short	tpage;
	u_short	clut;
} MESH;
	
/*
 * prototypes
 */
POLY_FT4 *meshRotTransPers(u_long *ot, int otsize, MESH *mesh, POLY_FT4 *heap);

/*
 * adaptive division functions (see div.c)
 */
void divPolyClip(RECT *rect, int t, int n);
POLY_FT4 *divPolyFT4(u_long *_ot,
		     SVECTOR *x0, SVECTOR *x1, SVECTOR *x2, SVECTOR *x3,
		     POLY_FT4 *_tmpl, POLY_FT4 *_heap, u_long *scratch);




