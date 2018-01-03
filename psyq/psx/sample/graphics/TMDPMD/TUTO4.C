/* $PSLibId: Run-time Library Release 4.4$ */
/*			    tuto4
 */			
/*	display many TMD object as PMD (POLY_F3 surface, without lighting) */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	----------------------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	2.00		Jul,14,1994	suzu	(using PMD)
 *	2.01		Mar,06,1997	sachiko	(added autopad)
 */

#include <sys/types.h>	
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/*
 * Triangle primitive buffer. This buffer should be allocaed dynamically
 * according to total primitive size by malloc() */
#define MAX_POLY	512			/* max polygon (per object) */

/* object definition */
typedef struct {
	MATRIX		m;	/* local-local matrix*/
	MATRIX		lw;	/* local-world matrix*/
	SVECTOR		v;	/* local vector*/
	int		n;	/* number of polygons*/
	struct {		/* polygon information*/
		POLY_FT3	prim[2];
		SVECTOR		v0, v1, v2;
	} p[MAX_POLY];
} OBJ_FT3;

#define SCR_Z		1024		/* projection*/
#define OTLENGTH	12		/* depth of OT*/
#define OTSIZE		(1<<OTLENGTH)	/* depth of OT*/
#define MAX_OBJ		16		/* max object*/

#define MODELADDR	((u_long *)0x80120000)	/* TMD address */
#define TEXADDR		((u_long *)0x80140000)	/* TIM address */

static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m);
static void set_position(OBJ_FT3 *obj, int n);
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id);
static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);

void tuto4(void)
{
	static SVECTOR	ang = {0,0,0};	/* self-rotation angle */
	MATRIX	rot;			/* self-rotation angle*/
	MATRIX	ws;			/* world matrix*/
	OBJ_FT3	obj[MAX_OBJ];		/* object*/
	u_long	ot[2][OTSIZE];		/* OT*/
	int	nobj = 1;		/* number of obj*/
	int	id   = 0;		/* buffer ID*/
	int	i, n; 			/* work */
	
	/* initialize frame double buffer*/
	db_init(640, 480/*240*/, SCR_Z, 60, 120, 120);	

	/* load TIM to frame buffer*/
	loadTIM(TEXADDR);	

	/* read each TMD*/
	for (i = 0; i < MAX_OBJ; i++) 
		loadTMD_FT3(MODELADDR, &obj[i]);	
	
	/* layout each object*/
	set_position(obj, 0);		
	
	/* start display */
	SetDispMask(1);			
	
	/* main loop*/
	while ((nobj = pad_read(obj, nobj, &ws)) != -1) {
		
		/* swap primitive buffer ID*/
		id = id? 0: 1;
		
		/* clear OT*/
		ClearOTagR(ot[id], OTSIZE);			

		/* rotate matrix of the earth*/
		ang.vy += 32;
		RotMatrix(&ang, &rot);
		
		/* set primitives*/
		for (i = 0; i < nobj; i++) {
			MulMatrix0(&obj[i].m, &rot, &obj[i].lw);
			add_OBJ_FT3(&ws, ot[id], &obj[i], id);
		}
		
		/* print debug information*/
		FntPrint("polygon=%d\n", obj[0].n);
		FntPrint("objects=%d\n", nobj);
		FntPrint("total  =%d\n", obj[0].n*nobj);
		
		/* draw OT and swap frame double buffer */
		db_swap(&ot[id][OTSIZE-1]);
	}
	DrawSync(0);
	return;
}

/*
 * Read controler and set world-screen matrix */	
static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m)
{
	static SVECTOR	ang   = {0, 512, 512};
	static VECTOR	vec   = {0, 0, SCR_Z*3/2};
	static int	scale = ONE;
	static int	opadd = 0;
	
	VECTOR	svec;
	int 	padd = PadRead(1); 
	
	if (padd & PADselect)	return(-1);
	if (padd & PADRup)	ang.vx -= 8;
	if (padd & PADRdown)	ang.vx += 8;
	if (padd & PADRright)	ang.vy -= 8;
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADR1) 	ang.vz += 8;
	if (padd & PADR2)	ang.vz -= 8;

	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;
	
	if ((opadd==0) && (padd & PADLup))	set_position(obj, nobj++);
	if ((opadd==0) && (padd & PADLdown))	nobj--;
	
	limitRange(nobj, 1, MAX_OBJ-1);
	opadd = padd;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, m);	
	
#ifdef ROTATE_YORSELF	/* rotate your camera*/
	{
		VECTOR	vec2;
		ApplyMatrixLV(m, &vec, &vec2);
		TransMatrix(m, &vec2);
		dumpVector("vec2=", &vec2);
	}
#else			/* rotate the world*/
	TransMatrix(m, &vec);	
#endif
	/* set world screen matrix with aspecto ratio correction */
	db_set_matrix(m);
	/*
	SetRotMatrix(m);
	SetTransMatrix(m);
	*/
	return(nobj);
}


/*
 * layout many object in the world-coordinate.
 * Since the position of each object is determined by rand(),
 * two objects may be located at the same position. */	
#define UNIT	400		/* resolution*/

static void set_position(OBJ_FT3 *obj, int n)
{
	SVECTOR	ang;

	static loc_tab[][3] = {
		 0, 0, 0,
		 1, 0, 0,	0, 1, 0,	 0, 0, 1,
		-1, 0, 0,	0,-1, 0,	 0, 0,-1,
		 1, 1, 0,	0, 1, 1,	 1, 0, 1,
		-1,-1, 0,	0,-1,-1,	-1, 0,-1,
		 1,-1, 0,	0,-1, 1,	-1, 0, 1,
		-1, 1, 0,	0, 1,-1,	 1, 0,-1,
	};
	
	/* set axis of each earth*/
	ang.vx = rand()%4096;
	ang.vy = rand()%4096;
	ang.vz = rand()%4096;
	RotMatrix(&ang, &obj[n].m);	
	
	/* set position of each earth*/
	obj[n].lw.t[0] = loc_tab[n][0]*UNIT;
	obj[n].lw.t[1] = loc_tab[n][1]*UNIT;
	obj[n].lw.t[2] = loc_tab[n][2]*UNIT;
}

/*
 * append object to OT*/
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id)
{
	MATRIX	ls;		/* local-screen matrix */
	
	/* push current matrix*/
	PushMatrix();				

	/* make local-screen coordinate */
	CompMatrix(ws, &obj->lw, &ls);

	/* set matrix*/
	SetRotMatrix(&ls);		/* set matrix */
	SetTransMatrix(&ls);		/* set vector */
	
	/* rotate-translate-perspective translation*/
	RotPMD_FT3((long *)&obj->n, ot, OTLENGTH, id, 0);

	/* recover old matrix*/
	PopMatrix();
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
	int		col, i, n_prim = 0;

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
		
		/* lighting*/
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(&obj->p[i].prim[0], col, col, col);
		
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
