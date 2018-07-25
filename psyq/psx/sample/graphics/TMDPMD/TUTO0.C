/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto0
 */			
/*	   TMD viewer prototype (without shading, without texture) */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	---------------------------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	1.10		Jan,22,1996	suzu	(English comment)
 *	1.20		Mar,07,1997	sachiko	(added autopad)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "rtp.h"

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z		1024	/* projection*/
#define OTSIZE		4096	/* depth of OT*/
#define MAX_POLY	3000	/* max polygon*/

#define MODELADDR	((u_long *)0x80100000)	/* TMD address */

/*
 * Vertex database of triangle primitive  */
typedef struct {
	SVECTOR	n0;		/* normal*/
	SVECTOR	v0, v1, v2;	/* vertex*/
} VERT_F3;

/*
 * Triangle primitive buffer. This buffer should be allocaed dynamically
 * according to total primitive size by malloc() */
typedef struct {
	int		n;		/* primitive number*/
	VERT_F3		vert[MAX_POLY];	/* vertex*/
	POLY_F3		prim[2][MAX_POLY];/* primitive*/
} OBJ_F3;

static int pad_read(int nprim);
int loadTMD_F3(u_long *tmd, OBJ_F3 *obj);

void tuto0(void)
{
	static OBJ_F3	obj;			/* object */
	static u_long	otbuf[2][OTSIZE];	/* OT */
	u_long		*ot;			/* current OT */
	int		id = 0;			/* primitive buffer ID */
	VERT_F3		*vp;			/* work */
	POLY_F3		*pp;			/* work */
	int		nprim;			/* work */
	int		i; 			/* work */

	/* initialize*/
	db_init(640, 480, SCR_Z, 60, 120, 120);	
	
	/* read TMD*/
	loadTMD_F3(MODELADDR, &obj);		
	
	/* start display*/
	SetDispMask(1);				

	/* main loop*/
	nprim = obj.n;
	while ((nprim = pad_read(nprim)) != -1) {
		
		/* clip max primitive in [0,max_nprim] */
		limitRange(nprim, 0, obj.n);

		/* swap primitive buffer ID*/
		id = id? 0: 1;
		ot = otbuf[id];
		
		/* clear OT*/
		ClearOTagR(ot, OTSIZE);			

		/* set primitive vertex*/
		vp = obj.vert;
		pp = obj.prim[id];
		
		/* 3D operation*/
		for (i = 0; i < nprim; i++, vp++, pp++) {
			/* rotTransPers3 is macro. see rtp.h */
			pp = &obj.prim[id][i];
			rotTransPers3(ot, OTSIZE, pp,
				      &vp->v0, &vp->v1, &vp->v2);

		}
		
		/* print debug information*/
		FntPrint("total=%d\n", i);
		
		/* swap OT and primitive buffer */
		db_swap(ot+OTSIZE-1);
	}
	DrawSync(0);
	return;
}

static int pad_read(int nprim)
{
	static SVECTOR	ang   = {512, 512, 512};	/* rotate angle */
	static VECTOR	vec   = {0,     0, SCR_Z};	
	static MATRIX	m;				/* matrix */
	static int	scale = ONE/4;
	
	VECTOR	svec;
	int 	padd = PadRead(1);

	if (padd & PADselect)	return(-1);
	if (padd & PADLup) 	nprim += 4;
	if (padd & PADLdown)	nprim -= 4;
	if (padd & PADRup)	ang.vx -= 32;
	if (padd & PADRdown)	ang.vx += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADR1) 	ang.vz += 32;
	if (padd & PADR2)	ang.vz -= 32;

	if (padd & PADL1)	vec.vz += 32;
	if (padd & PADL2) 	vec.vz -= 32;

	ang.vz += 8;
	ang.vy += 8;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	
	ScaleMatrix(&m, &svec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(nprim);
}		

/*
 * load TMD*/
int loadTMD_F3(u_long *tmd, OBJ_F3 *obj)
{
	VERT_F3		*vert;
	POLY_F3		*prim0, *prim1;
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

	vert  = obj->vert;
	prim0 = obj->prim[0];
	prim1 = obj->prim[1];
	
	/* open TMD*/
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY)
		n_prim = MAX_POLY;

	/*
	 * Set unchanged member of primitive here to deliminate main
	 * memory write access */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {
		
		/* initialize primitive*/
		SetPolyF3(prim0);

		/* copy normal and vertex*/
		copyVector(&vert->n0, &tmdprim.n0);
		copyVector(&vert->v0, &tmdprim.x0);
		copyVector(&vert->v1, &tmdprim.x1);
		copyVector(&vert->v2, &tmdprim.x2);
		
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(prim0, col, col, col);
		
		/* duplicate primitive for primitive double buffering */  
		memcpy(prim1, prim0, sizeof(POLY_F3));
		vert++, prim0++, prim1++;
	}
	return(obj->n = i);
}
