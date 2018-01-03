/* $PSLibId: Run-time Library Release 4.4$ */
/*				tuto5
 */			
/*      TMD-PMD viewer prototype (light-source calculations performed, type FT3) */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	2.00		Jul,14,1994	suzu	(using PMD)
 *	2.10		Jul,14,1994	suzu	(with lighting)
 *	2.20		Mar,06,1997	sachiko	(added autopad)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define SCR_Z		1024		/* projection */
#define OTLENGTH	12		/* depth of OT */
#define OTSIZE		(1<<OTLENGTH)	/* depth of OT */
#define MAX_OBJ		16		/* max objects */
#define MODELADDR	(0x80120000)	/* TMD address in main memory */
#define TEXADDR		(0x80140000)	/* TIM address in main memory */

/*
 * triangle primitive buffer
 * This area may be allocated dynamically using malloc() according to 'n'. */
#define MAX_POLY	400			/* max polygon (per object) */
typedef struct {
	int	n;
	struct {			
		POLY_FT3	prim[2];
		SVECTOR v0, v1, v2;
	} p[MAX_POLY];
	SVECTOR	n0[MAX_POLY];			/* normal*/
} OBJ_FT3;

static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);
static int lightOBJ_FT3(OBJ_FT3 *obj, MATRIX *cmat, SVECTOR *lang, int ambi);
static int pad_read(OBJ_FT3 *obj);

void tuto5(void)
{
	static OBJ_FT3	obj;			/* object*/
	static u_long	otbuf[2][OTSIZE];	/* OT buffer*/
	int		id = 0;			/* packet ID */
	int		i, nprim;
		
	db_init(640, 480, SCR_Z, 60, 120, 120);	
	loadTIM((u_long *)TEXADDR);	

	/* load TMD*/
	nprim = loadTMD_FT3((u_long *)MODELADDR, &obj);

	/* start display*/
	SetDispMask(1);			

	/* main loop*/
	while (pad_read(&obj) == 0) {
		
		/* swap packet ID*/
		id = id? 0: 1;
		
		/* clear OT */
		ClearOTagR(otbuf[id], OTSIZE);			

		/* rotation PMD object*/
		RotPMD_FT3((long *)&obj, otbuf[id], OTLENGTH, id, 0);

		/* print*/
		FntPrint("PMD with lighting\n");
		
		/* draw OT and swap frame double buffer */
		db_swap(otbuf[id]+OTSIZE-1);
	}
	DrawSync(0);
	return;
}

/*
 * Load TIM data from main memory to the frame buffer */	
static void loadTIM(u_long *addr)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr) {	/* load CLUT (if needed) */
			setSTP(image.caddr, image.crect->w);
			LoadImage(image.crect, image.caddr);
		}
		if (image.paddr) 	/* load texture pattern */
			LoadImage(image.prect, image.paddr);
	}
}
	
/*
 * change STP on texture pattern to inhibit transparent color */	
static void setSTP(u_long *col, int n)
{
	n /= 2;  
	while (n--) 
		*col++ |= 0x80008000;
}

/*
 * parse TMD*/
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj)
{
	TMD_PRIM	tmdprim;
	int		i, n_prim = 0;

	/* open TMD*/
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY) 
		n_prim = MAX_POLY;
	
	/*
	 * Set unchanged member of primitive here to deliminate main
	 * memory write access */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {

		/* initialize primitive*/
		SetPolyFT3(&obj->p[i].prim[0]);

		/* copy normal and vertex*/
		copyVector(&obj->p[i].v0, &tmdprim.x0);
		copyVector(&obj->p[i].v1, &tmdprim.x1);
		copyVector(&obj->p[i].v2, &tmdprim.x2);
		
		/* set normal*/
		copyVector(&obj->n0[i],   &tmdprim.n0);
		
		/* copy texture point because this point never changes
		 * by rotation */
		setUV3(&obj->p[i].prim[0], 
		       tmdprim.u0, tmdprim.v0,
		       tmdprim.u1, tmdprim.v1,
		       tmdprim.u2, tmdprim.v2);
		
		/* copy tpage and clut */
		obj->p[i].prim[0].tpage = tmdprim.tpage;
		obj->p[i].prim[0].clut  = tmdprim.clut;

		/* duplicate primitive for primitive double buffering */  
		memcpy(&obj->p[i].prim[1],
		       &obj->p[i].prim[0], sizeof(POLY_FT3));
		
	}
	return(obj->n = i);
}

/*
 * calculate lighting*/
static int lightOBJ_FT3(OBJ_FT3 *obj, MATRIX *cmat, SVECTOR *lang, int ambi)
{
	int	i;	
	MATRIX	lmat;
	P_CODE	csrc, cdst;
	SVECTOR	*n;
	
	/* set ambient color*/
	SetBackColor(ambi, ambi, ambi);	

	/* set color matrix*/
	SetColorMatrix(cmat);		
	
	/* set local light matrix*/
	RotMatrix(lang, &lmat);		
	SetLightMatrix(&lmat);		
	
	/* material color is (255,255,255)*/
	setRGB0(&csrc, 255, 255, 255);		
	
	/* preserve primitive code because NormalColorCol destroys it */
	csrc.code = obj->p[0].prim[0].code;	

	/* lighing calculation*/
	for (i = 0, n = obj->n0; i < obj->n; i++, n++) {
		NormalColorCol(n, (CVECTOR *)&csrc, (CVECTOR *)&cdst);
		*(u_long *)(&obj->p[i].prim[0].r0) = *(u_long *)(&cdst);
		*(u_long *)(&obj->p[i].prim[1].r0) = *(u_long *)(&cdst);
	}
	return(0);
}
	
	       
/*
 * analyze controller*/	
static MATRIX	cmat = {		/* light source*/
/* 	 #0,	#1,	#2, */
	ONE,	0,	0,	/* R */
	0,	ONE,	0, 	/* G */
	0,	0,	ONE,	/* B */
};
	
static int pad_read(OBJ_FT3 *obj)
{
	static int	light = 1;
	static SVECTOR	ang   = {512, 512, 512};	/* rotate angle */
	static VECTOR	vec   = {0,     0, SCR_Z*2};	/* position */
	static SVECTOR	lang  = {ONE, ONE, ONE};	/* light angle */
	static MATRIX	m;				/* matrix */
	static int	scale = 2*ONE;			/* scale */
	static int	frame = 0;
	
	VECTOR	svec;
	int 	padd = PadRead(1);

	if (padd & PADselect)	return(-1);
	if (padd & PADRup)	ang.vx -= 32;
	if (padd & PADRdown)	ang.vx += 32;
	if (padd & PADRright)	ang.vy -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADR1) 	ang.vz += 8;
	if (padd & PADR2)	ang.vz -= 8;
	
	if (padd & PADL1)	vec.vz += 32;
	if (padd & PADL2) 	vec.vz -= 32;
	
	if (frame%2 == 0 && (padd & (PADLup|PADLdown|PADLleft|PADLright)))
		light = 1;
	    
	if (padd & PADLup)	lang.vx -= 32;
	if (padd & PADLdown)	lang.vx += 32;
	if (padd & PADLright)	lang.vy -= 32;
	if (padd & PADLleft) 	lang.vy += 32;
	
	ang.vz += 8;
	ang.vy += 8;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, &m);	
	TransMatrix(&m, &vec);	
	ScaleMatrix(&m, &svec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	/* Lighting is changed only when light flag is 1. Lighting doesn't
	 * have to be changed by each frame.   */
	if (light) {
		light = 0;
		lightOBJ_FT3(obj, &cmat, &lang, 0);
	}

	frame++;
	return(0);
}		


