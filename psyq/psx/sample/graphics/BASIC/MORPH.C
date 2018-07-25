/* $PSLibId: Run-time Library Release 4.4$ */
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
 *	2.01		Mar, 5,1997	sachiko	(added autopad)
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/*
 * Mesh Prototype
 */
typedef struct {
	SVECTOR	x3;		/* 3D point */
	SVECTOR	x2;		/* 2D point (after perspective trans.) */
	/*int	sz;		/* otz */
} MESH;

void mesh_scroll(MESH *mp, int mx, int my, int rx, int ry);
void mesh_init(MESH *mp, int mx, int my, int width, int height);
void mesh_RotTransPers(MESH *mp, int mx, int my);
void mesh_AddPrim(u_long *ot,int otsize,POLY_FT4 *p,MESH *mp,int mx,int my);
void mesh_morf(MESH *mp, int mx, int my, int cx, int cy, int h);

/*
 * OT Parameters
 */
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

void Morph(void)
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
				DrawSync(0);
				return;
			}
		}
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
		/* draw */
		Status == 0? draw(cdb): expl(cdb);
		
		DrawSync(0);		/* wait for end of drawing */
		VSync(0);		/* wait for the next V-BLNK */

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
	
	u_long	padd;
	int	ret = 0;

	padd = PadRead(1);

	if (padd & (PADR1|PADR2|PADL1|PADL2)) {	/* move morphing position */
		if (padd & PADLup)	Mpos[1]--;
		if (padd & PADLdown)	Mpos[1]++;
		if (padd & PADLleft) 	Mpos[0]--;
		if (padd & PADLright)	Mpos[0]++;
		
		limitRange(Mpos[0], 0, PT_NX-1);
		limitRange(Mpos[1], 0, PT_NY-1);
	}
	else {					
		/* rotate */
		if (padd & PADLup)	ang.vx += DANG;
		if (padd & PADLdown)	ang.vx -= DANG;
		if (padd & PADLleft) 	ang.vy += DANG;
		if (padd & PADLright)	ang.vy -= DANG;

		/* move scroll position */
		if (padd & PADRup)	Srate[1] -= DSCRL;
		if (padd & PADRdown)	Srate[1] += DSCRL;
		if (padd & PADRleft) 	Srate[0] -= DSCRL;
		if (padd & PADRright)	Srate[0] += DSCRL;
		
		limitRange(Srate[0], -240, 240);
		limitRange(Srate[1], -240, 240);
	}
	
	/* update morphing depth */
	if (padd & PADR1)	Mheight -= DMORF;
	if (padd & PADR2)	Mheight += DMORF;
	limitRange(Mheight, -128, 128);

	/* reset */
	if (padd & (PADstart)) {
		Srate[0] = Srate[1] = 0;
		setVector(&ang, 0, 0, 0);
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


/****************************************************************************
 *  
 *			2D effects
 *
 ****************************************************************************/

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
	int	i, x, y;
	int	sx, sy;
	int	dsx = 4096/mx, dsy = 4096/my;
	int	dx, dy;
	
	for (sy = y = 0; y < my; y++, sy += dsy) {
		dx = rx*rsin(sy)>>12;
		for (sx = x = 0; x < mx; x++, sx += dsx, mp++) {
			dy = ry*rsin(sx)>>12;
			mp->x3.vx += dx;
			mp->x3.vy += dy;
		}
	}
}
/****************************************************************************
 *  
 *		Simple Basic Mesh Handler
 *
 ****************************************************************************/

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

