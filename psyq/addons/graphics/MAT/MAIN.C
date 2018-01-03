/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*			mat: sample program
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Aug,31,1993	suzu
 *	2.00		Nov,17,1993	suzu	(using 'libgpu)
 *	3.00		Dec.27.1993	suzu	(rewrite)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Constants
 */
#define	FSIZE		600		/* floor size */
#define	HFSIZE		(FSIZE/2)	/* 1/2 of floor size */
#define	FY		128		/* floor height */
#define MAXMAT		30		/* maximum 'Matchang' number */
#define MATSIZ		80		/* 'Matchang' scale */
#define DEPTH		512		/* distance to the rotation center */
#define SCR_Z		128		/* distance to screen */
#define OTSIZE		((DEPTH*2)>>2)	/* ordering table size */

/*
 * Primitive Buffer
 */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_F4		floor;			/* floor */
	POLY_FT4	shadow[MAXMAT];		/* shadows */
	POLY_FT4	mat[MAXMAT];		/* Matchang's */
} DB;

/*
 * Position Buffer
 */
typedef struct {
	SVECTOR	x;		/* position */
	SVECTOR	v;		/* verocity */
	short	id, pad; 	/* sprite pattern ID */
} POS;

/*
 * Texture Info.
 */
typedef struct {
	u_char	u, v;		
	u_short	tpage;		
	u_short	clut;		/* Matchag's CLUT */
	u_short sclut;		/* shadow's CLUT */
} TEX;

u_long	out_packet[0x1000];	/* packet buffer */

static void move_floor(u_long *ot, POLY_F4 *floor);
static void move_mat_uv(TEX *tex, POLY_FT4 *mat, POLY_FT4 *shadow);
static void move_mat_pos(u_long *ot, POS *pos, POLY_FT4 *mat,POLY_FT4 *shadow);
static void init_tex(TEX *tex);
static void init_prim(DB *db);
static void init_point(POS *pos);
static int pad_read(int n);

main()
{
	static POS pos[MAXMAT];	/* Matchang position table */
	static TEX tex[64];	/* Matchang has 64 animation patterns */
	
	DB	*db;			/* double buffer */
	DB	*cdb;			/* current double buffer */
	int	nmat = 1;		/* Matchan number */
	int	i;			/* work */

	PadInit(0);		/* reset PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(160, 120);	/* set geometry origin as (320,120) */
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	db = (DB *)out_packet;	/* allocate output packet buffer */
	
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);

	init_tex(tex);		/* initialize texture attributes */
	init_prim(&db[0]);	/* initialize primitive buffers #0 */
	init_prim(&db[1]);	/* initialize primitive buffers #1 */
	init_point(pos);	/* set initial geometries */

	/* display */
	SetDispMask(1);
	
	while ((nmat = pad_read(nmat)) > 0) {
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
		move_floor(cdb->ot, &cdb->floor);	/* update floor */
		
		/* update primitives */
		for (i = 0; i < nmat; i++) {
			move_mat_uv(&tex[pos[i].id],
				    &cdb->mat[i], &cdb->shadow[i]);
			move_mat_pos(cdb->ot, &pos[i],
				     &cdb->mat[i], &cdb->shadow[i]);
		}

		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environnment */
		PutDispEnv(&cdb->disp); /* update display environnment */
		DrawOTag(cdb->ot);	  /* draw */
	}
	PadStop();
	StopCallback();
	return(0);
}

static void move_floor(u_long *ot, POLY_F4 *floor)
{
	static SVECTOR fv[] = {			/* floor world vertex */
		{-HFSIZE, FY,  HFSIZE, 0}, { HFSIZE, FY,  HFSIZE, 0},
		{-HFSIZE, FY, -HFSIZE, 0}, { HFSIZE, FY, -HFSIZE, 0},
	};

	long	d0, flg;

	RotAverage4(&fv[0], &fv[1], &fv[2], &fv[3],
		    (long *)&floor->x0, (long *)&floor->x1, 
		    (long *)&floor->x2, (long *)&floor->x3, &d0, &flg);
	AddPrim(ot+1, floor);
}

static void move_mat_uv(TEX *tex, POLY_FT4 *mat, POLY_FT4 *shadow)
{
	/* update textures */
	mat->tpage =  tex->tpage;
	mat->clut  =  tex->clut;
	setUV4(mat, tex->u, tex->v,    tex->u+63, tex->v,
	            tex->u, tex->v+63, tex->u+63, tex->v+63);

	shadow->tpage = tex->tpage;
	shadow->clut  = tex->sclut;
	setUV4(shadow, tex->u, tex->v,    tex->u+63, tex->v,
	               tex->u, tex->v+63, tex->u+63, tex->v+63);
	
}

static void move_mat_pos(u_long *ot, POS *pos, POLY_FT4 *mat, POLY_FT4 *shadow)
{
	
	static MATRIX	mmat = {	/* Matchang  matrix */
		ONE,	  0,	0,	/* (for vertical polygon) */
		0,	ONE,	0,
		0,	  0,  ONE,
	};
	static MATRIX	smat;		/* shadow's matrix */
					   
	static SVECTOR mv[] = {		/* Matchang vertex */
		{-MATSIZ,-MATSIZ, 0, 0}, {MATSIZ,-MATSIZ, 0, 0},
		{-MATSIZ, MATSIZ, 0, 0}, {MATSIZ, MATSIZ, 0, 0},
	};

	static SVECTOR sv[] = {		/* shadow's vertex */
		{-MATSIZ, 0,-MATSIZ, 0},{ MATSIZ, 0,-MATSIZ, 0},
		{-MATSIZ, 0, MATSIZ, 0},{ MATSIZ, 0, MATSIZ, 0},
	};
	
	SVECTOR	sx;			/* shadow's postion */
	long	id, otz, d0, flg;
	int	i;

	/* frame and position update */
	pos->id     = (pos->id+1)%64;
	pos->x.vx  += pos->v.vx;
	pos->x.vy  += pos->v.vy;
	pos->x.vz  += pos->v.vz;
	pos->v.vy  += 1;
	
	/* reflection */
        if (pos->x.vy > FY) pos->v.vy = -rand()%10-15;
        if (pos->x.vx > HFSIZE || pos->x.vx < -HFSIZE) pos->v.vx *= -1;
        if (pos->x.vz > HFSIZE || pos->x.vz < -HFSIZE) pos->v.vz *= -1;

	/* move matchan */
	PushMatrix();
	
	/* world-screen translation */
	RotTrans(&pos->x, (VECTOR *)mmat.t, &flg);	
	SetRotMatrix(&mmat);			/* Matchang never rotates! */
	SetTransMatrix(&mmat);			/* Matchang only moves */
	
	otz = OTSIZE - RotAverage4(&mv[0], &mv[1], &mv[2], &mv[3],
			   (long *)&mat->x0, (long *)&mat->x1, 
			   (long *)&mat->x2, (long *)&mat->x3, &d0, &flg);
	
    	limitRange(otz, 2, OTSIZE);
	AddPrim(ot+otz, mat);
	PopMatrix();
	
	/* move shadow */
	PushMatrix();
	sx.vx = pos->x.vx, sx.vy = FY, sx.vz = pos->x.vz;
	RotTrans(&sx, (VECTOR *)smat.t, &flg);	/* world-screen translation */
	SetTransMatrix(&smat);		

	RotAverage4(&sv[0], &sv[1], &sv[2], &sv[3],
		(long *)&shadow->x0, (long *)&shadow->x1, 
		(long *)&shadow->x2, (long *)&shadow->x3, &d0, &flg);
	
	AddPrim(ot+2, shadow);

	PopMatrix();
}

/*
 * Initialize textures
 *	This program uses 64 patterns of 64x64 texture image.
 *	Thiese patterns requires 4 pages of a 256x256 texture page.
 *	Frame allocation is:
 *		frame  0-15	texture page #0
 *		frame 16-31	texture page #1
 *		frame 32-47	texture page #2
 *		frame 48-63	texture page #3
 */
static void init_tex(TEX *tex)
{
	/* 'matchang' texture database
	 *	0x000-0x07f:	CLUT  (256x2byte entry)
	 *	0x200-		INDEX (4bit mode:256x256)
	 */
	extern	u_long	mat0[];		/* Matchang of texture page #0 */
	extern	u_long	mat1[];		/* Matchang of texture page #1 */
	extern	u_long	mat2[];		/* Matchang of texture page #2 */
	extern	u_long	mat3[];		/* Matchang of texture page #3 */

	u_short	tpage[4];		/* texture page ID for matchang */
	u_short	clut[4];		/* texture CLUT ID for matchang */
	u_short	sclut;			/* shadow CLUT ID */
	u_short	shadow[256];		/* shadow colors */
	
        int     uvp;			/* texture ID in texture page */
        int     texp;			/* texture page ID */
        int     i;

	/* make shadow color */
	shadow[0] = 0x0000;		/* transparent */
	for(i = 1; i < 256; i++)
		shadow[i] = 0x8000;	/* not transparent but black */

	/* load texture page */
	tpage[0] = LoadTPage(mat0+0x80, 0, 0, 640,  0, 256,256);
	tpage[1] = LoadTPage(mat1+0x80, 0, 0, 640,256, 256,256);
	tpage[2] = LoadTPage(mat2+0x80, 0, 0, 704,  0, 256,256);
	tpage[3] = LoadTPage(mat3+0x80, 0, 0, 704,256, 256,256);
	
	/* load clut */
	clut[0] = LoadClut(mat0,             0,500);
	clut[1] = LoadClut(mat1,             0,501);
	clut[2] = LoadClut(mat2,             0,502);
	clut[3] = LoadClut(mat3,             0,503);
	sclut   = LoadClut((u_long *)shadow, 0,504);
	
        /* set texture animation */
        for (i = 0; i < 64; i++) {
                uvp  = i%16, texp = i/16;

                tex[i].u     = 64*(uvp%4);
                tex[i].v     = 64*(uvp/4);
                tex[i].tpage = tpage[texp];
                tex[i].clut  = clut[texp];
                tex[i].sclut = sclut;
        }
}	

static void init_prim(DB *db)
{
	POLY_FT4	*mp, *sp;
	int		i;
	
	/* set BG */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);
	
	/* set floor */
	SetPolyF4(&db->floor);
	setRGB0(&db->floor, 60, 60, 60);

	/* set mat and shadow */
	mp = db->mat, sp = db->shadow;
	for (i = 0; i < MAXMAT; i++, mp++, sp++) {
		SetPolyFT4(mp),	SetSemiTrans(mp, 0), SetShadeTex(mp, 1);
		SetPolyFT4(sp),	SetSemiTrans(sp, 0), SetShadeTex(sp, 1);
	}
}	

/*
 * Initialize sprite position and verocity
 */
static void init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXMAT; i++) {
		pos[i].x.vx = rand()%FSIZE - FSIZE/2;
		pos[i].x.vy = FY;
		pos[i].x.vz = rand()%FSIZE - FSIZE/2;
		pos[i].v.vx = rand()%4+1;
		pos[i].v.vy = -rand()%10-15;
		pos[i].v.vz = rand()%4+1;
		pos[i].id  = 0;
	}
}

/*
 * Read controll-pad
 */
static MATRIX	m;
static int pad_read(int n)
{
	static SVECTOR	ang = {0, 0, 0};
	static VECTOR	vec = {0, 0, DEPTH};
	
	u_long	padd = PadRead(1);
	
	if (padd & PADLup)	ang.vx += 64;
	if (padd & PADLdown)	ang.vx -= 64;
	if (padd & PADLleft) 	ang.vy += 64;
	if (padd & PADLright)	ang.vy -= 64;
	if (padd & PADRup)	n++;
	if (padd & PADRdown)	n--;
	if (padd & PADk) 	return(-1);
	
	limitRange(n,      1, MAXMAT-1);

	limitRange(ang.vx, 0, 1024);		/* for MW3 */

	RotMatrix(&ang, &m);
	TransMatrix(&m, &vec);
	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(n);
}		
