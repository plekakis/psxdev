/* $PSLibId: Runtime Library Release 3.6$ */
/*
 *			2D: sample program
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
#include "mesh.h"

#define SCR_Z	1024			/* distance to screen */
#define OTSIZE	SCR_Z			/* ordering table size */

/*
 * 640x240 image is divided in many 16x16cells, and image data is handled as
 * a set of meshed POLY_FT4 primitives
 */
#define PT_UX	16			/* cell size (in UV space) */
#define PT_UY	16

#define PT_NX	(512/PT_UX)		/* number of cells */
#define PT_NY	(240/PT_UY)

/*
 * Grobal definitions
 */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_FT4	cell[PT_NY*PT_NX];	/* mesh cell */
} DB;

static int	Status;				/* status */
static MESH	Mesh[PT_NY+1][PT_NX+1];		/* vertex mesh array */
static int	Mpos[2]  = {PT_NX/2, PT_NY/2};	/* morphing center position */
static int	Mheight  = 0;			/* morphing height */
static int	Srate[2] = {0, 0};		/* 'line scroll' rate (x, y) */
static SVECTOR	ang      = {0, 0, 0};		/* rotate angle */
static VECTOR	vec      = {0, 0, SCR_Z};	/* distance from eye positon */

static void draw(DB *db);	
static void init_prim(DB *db);
static pad_read(void);
static void expl_init(void);
static void expl(DB *db);

main()
{
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */
	u_long	*ot;		/* current OT */
	int	ret;
	
	/* initialize grobal parameters */
	Status   = 0;		/* status 0:normal, 1:explosion */
	Mpos[0]  = PT_NX/2;	/* morphing position is set to center */
	Mpos[1]  = PT_NY/2;	 
	Mheight  = 0;		/* mophing rate is 0 (do nothing) */
	Srate[0] = 0;		/* line scroll rate is 0 (do nothing) */
	Srate[1] = 0;
	setVector(&ang, 0, 0, 0);
	
	/* reset graphic system */
	PadInit(0);		/* reset PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(320, 120);	/* set geometry origin as (320,120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 640, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 640, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 640, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   640, 240);
	
	init_prim(&db[0]);	/* initialize primitive buffers #0 */
	init_prim(&db[1]);	/* initialize primitive buffers #1 */
	
	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	/* main loop */
	while (1) {
		if (pad_read() == -1) {		/* if END key is pressed */
			if (Status == 0) {
				Status = 1;	/* move to explosion mode */
				expl_init();
			}
			else {		/* exit */
				PadStop();
				ResetGraph(3);
				StopCallback();
				return;
			}
		}
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
		/* draw */
		Status == 0? draw(cdb): expl(cdb);
		
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		DrawOTag(cdb->ot);	  /* draw */
	}
}

static void draw(DB *db)		/* normal draw */
{
	mesh_init(&Mesh[0][0], PT_NX+1, PT_NY+1, 640, 480);
	mesh_morf(&Mesh[0][0], PT_NX+1, PT_NY+1, Mpos[0], Mpos[1], Mheight);
	mesh_scroll(&Mesh[0][0], PT_NX+1, PT_NY+1, Srate[0], Srate[1]);
	mesh_RotTransPers(&Mesh[0][0], PT_NX+1, PT_NY+1);
	mesh_AddPrim(db->ot, OTSIZE, db->cell, &Mesh[0][0], PT_NX+1, PT_NY+1);
}

static void init_prim(DB *db)
{
	extern	u_long	glass[];	/* glass 2D image */
	
	static RECT texp  = {640,   0, 256, 256};
	static RECT clutp = {  0, 480, 256,   1};
	
	int		x, y, ux, uy, tx, ty, i;
	u_short		tpage0, tpage1;		/* texture page (2pages) */
	u_short		clut;			/* texture CLUT (256colors) */
	POLY_FT4	*p;
	
	LoadImage(&texp,  glass+0x80);	/* 512x256 8bit texture (2 pages) */
	tpage0 = GetTPage(1, 0, 640,     0);	/* page #0 address */
	tpage1 = GetTPage(1, 0, 640+128, 0); 	/* page #1 address */

	clut   = LoadClut(glass, 0, 480);	/* load CLUT */
			  
	/* init background */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);	/* (r,g,b) = (60,120,120) */

	/* initialize meshed UV */
	for (p = db->cell, y = 0, uy = 0; y < PT_NY; y++, uy += PT_UY)
		for (x = 0, ux = 0; x < PT_NX; x++, ux += PT_UX, p++) {

			SetPolyFT4(p);		/* FlatTexture Quadrangle */
			SetShadeTex(p, 1);	/* no shade-texture */
			
			setUV4(p, ux, uy,         ux+PT_UX-1, uy,
			          ux, uy+PT_UY-1, ux+PT_UX-1, uy+PT_UY-1);
			
			p->tpage = ux<255? tpage0: tpage1;
			p->clut  = clut;
		}
}

#define DANG	32		/* rotate speed */
#define DMORF	8		/* morphing speed */
#define DSCRL	4		/* scroll speed */

static pad_read(void)
{
	static MATRIX	m;
	static int	opadd = 0;
	
	u_long	padd = PadRead(1);
	int	ret = 0;

	if (padd & PADRup)	ang.vx += DANG;
	if (padd & PADRdown)	ang.vx -= DANG;
	if (padd & PADRleft) 	ang.vy += DANG;
	if (padd & PADRright)	ang.vy -= DANG;

	if (padd & (PADR1|PADR2|PADL1|PADL2)) {	/* move morphing position */
		if (padd & PADLup)	Mpos[1]--;
		if (padd & PADLdown)	Mpos[1]++;
		if (padd & PADLleft) 	Mpos[0]--;
		if (padd & PADLright)	Mpos[0]++;
		
		limitRange(Mpos[0], 0, PT_NX-1);
		limitRange(Mpos[1], 0, PT_NY-1);
	}
	else {					/* move scroll position */
		if (padd & PADLup)	Srate[1] -= DSCRL;
		if (padd & PADLdown)	Srate[1] += DSCRL;
		if (padd & PADLleft) 	Srate[0] -= DSCRL;
		if (padd & PADLright)	Srate[0] += DSCRL;
		
		limitRange(Srate[0], -160, 160);
		limitRange(Srate[1], -160, 160);
	}
	
	/* update morphing depth */
	if (padd & PADR1)	Mheight -= DMORF;
	if (padd & PADR2)	Mheight += DMORF;
	limitRange(Mheight, -128, 128);

	/* reset */
	if (padd & (PADstart)) {
		Srate[0] = Srate[1] = 0;
		/*setVector(&ang, 0, 0, 0);*/
		Mheight = 0;
		Mpos[0] = PT_NX/2;
		Mpos[1] = PT_NY/2;
	}

	/* end */
	if (!(opadd & PADk) && (padd & PADk))
		ret = -1;
	
	/* update matrix */
	RotMatrix(&ang, &m);

	/* adjuct aspect ratio */
	m.m[1][0] /= 2;
	m.m[1][1] /= 2;
	m.m[1][2] /= 2;

	TransMatrix(&m, &vec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	opadd = padd;
	return(ret);
}		

/****************************************************************************
 *  
 *			Explosion
 *
 ****************************************************************************/

typedef struct {
	SVECTOR	x0, x1, x2, x3;		/* vertex position */
	SVECTOR	v;			/* verocity */
} POS;

POS Pos[PT_NX*PT_NY];

static void expl_init(void)
{
	int	x, y;
	POS	*p;
	
	/* take a snapshot of the morphed image vertexes */
	for (p = Pos, y = 0; y < PT_NY; y++)
	for (x = 0; x < PT_NX; x++,  p++) {
		memcpy(&p->x0, &Mesh[y  ][x  ].x3, sizeof(SVECTOR));
		memcpy(&p->x1, &Mesh[y  ][x+1].x3, sizeof(SVECTOR));
		memcpy(&p->x2, &Mesh[y+1][x  ].x3, sizeof(SVECTOR));
		memcpy(&p->x3, &Mesh[y+1][x+1].x3, sizeof(SVECTOR));
		
		p->v.vx = x*2 - PT_NX;
		p->v.vy = y*2 - PT_NY;
		p->v.vz = 0;
	}
}	

static void expl(DB *db)
{
	POLY_FT4	*pt;
	POS		*pp;
	int		i, j;
	int		p, otz, opz;
	long		dmy, flg;

	pt = db->cell;
	pp = Pos;
	
	for (i = 0; i < PT_NX*PT_NY; i++, pt++, pp++) {
		otz = RotAverage4(&pp->x0, &pp->x1, &pp->x2, &pp->x3,
			  (long *)&pt->x0, (long *)&pt->x1, 
			  (long *)&pt->x2, (long *)&pt->x3, &dmy, &flg);

		limitRange(otz, 0, OTSIZE-1);
		AddPrim(db->ot+(OTSIZE-otz), pt);
		
		addVector(&pp->x0, &pp->v);
		addVector(&pp->x1, &pp->v);
		addVector(&pp->x2, &pp->v);
		addVector(&pp->x3, &pp->v);
	}
}
