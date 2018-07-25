/* $PSLibId: Run-time Library Release 4.4$ */
/*                  balls:
 *
 *        Renders multiple balls bouncing on the screen
 *
 *        Copyright  (C)  1993 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   1.00      Aug,31,1993    suzu
 *   2.00      Nov,17,1993    suzu  (using 'libgpu) 
 *   3.00      Dec.27.1993    suzu  (rewrite) 
 *   3.01      Dec.27.1993    suzu  (for newpad) 
 *   3.02      Aug.31.1994    noda     (for KANJI) 
 *   4.00      May.22.1995    sachiko    (added comments in Japanese) 
 *   4.01      Mar. 5.1997    sachiko    (added autopad) 
 **/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/*
 * Kaji Printf */
#define KANJI

/*#define DEBUG 	/* for debug */

/*
 * Primitive Buffer */
#define OTSIZE	1		/* size of OT */
#define MAXOBJ	4000		/* max ball number */

typedef struct {
	DRAWENV	draw;		/* drawing environment */
	DISPENV	disp;		/* display environment */
	u_long	ot[OTSIZE];	/* OT entry */
	SPRT_16	sprt[MAXOBJ];	/* 16x16 sprite */
} DB;

/*
 * Position Buffer */
typedef struct {
	u_short x, y;		/* current point */
	u_short dx, dy;		/* verocity */
} POS;

/*
 * Display area */
#define	FRAME_X	320		/* frame size */
#define	FRAME_Y	240
#define WALL_X	(FRAME_X-16)	/* reflection point */
#define WALL_Y	(FRAME_Y-16)

/*
 * prototypes */
static void init_prim(DB *db);	
static int  pad_read(int n);	
static int  init_point(POS *pos);
static void cbvsync(void);

/*
 * Main */
void Balls(void)
{
	
	/* buffer storing ball coordinates and displacement distance */
	POS	pos[MAXOBJ];	
	
	/* double buffer */
	DB	db[2];		
	
	/* current double buffer */
	DB	*cdb;		

	/* object number */
	int	nobj = 1;	
	
	/* current OT */
	u_long	*ot;		
	
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
	/* set debug mode (0:off,1:monitor,2:dump) */
	SetGraphDebug(0);	
	
	/* set up V-sync callback function */
	VSyncCallback(cbvsync);	

	/* set up rendering/display environment for double buffering
        when rendering (0, 0) - (320,240), display (0,240) - (320,480) (db[0]) 
        when rendering (0,240) - (320,480), display (0,  0) - (320,240) (db[1])  */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init font environment */
#ifdef KANJI	/* KANJI */
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	/* load font pattern */
	FntLoad(960, 256);	
	
	/* set text window */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* initialize primitive buffer */
	init_prim(&db[0]);	
	
	/* initialize primitive buffer */
	init_prim(&db[1]);	
	
	/* set initial geometries */
	init_point(pos);	

	/* main loop */
	while ((nobj = pad_read(nobj)) > 0) {
		/* swap double buffer ID */
		cdb  = (cdb==db)? db+1: db;	
#ifdef DEBUG
		/* dump DB environment */
		DumpDrawEnv(&cdb->draw);
		DumpDispEnv(&cdb->disp);
		DumpTPage(cdb->draw.tpage);
#endif

 		/* clear ordering table */
		ClearOTag(cdb->ot, OTSIZE);

		/* update sprites */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* detect reflection */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* update vertex */
			setXY0(sp, x, y);	
			
			/* apend to OT */
			AddPrim(ot, sp);	
		}
		/* wait for end of drawing */
		DrawSync(0);		
		
		/* get CPU consuming time */
		/* cnt = VSync(1); */

		/* for 30fps operation */
		/* cnt = VSync(2); */	

		/* wait for Vsync */
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */

		/* swap double buffer */
		/* display environment */
		PutDispEnv(&cdb->disp); 
		
		/* drawing environment */
		PutDrawEnv(&cdb->draw); 
		
		/* draw OT */
		DrawOTag(cdb->ot);	
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/* print message */
#ifdef KANJI
		KanjiFntPrint("‹Ê‚Ì”%d\n", nobj);
		KanjiFntPrint("ŽžŠÔ=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n", cnt);
		FntFlush(-1);
	}
	/* clear Callback */
	VSyncCallback(0);
	
	DrawSync(0);
	return;
}

/*
 * Initialize drawing Primitives */
#include "balltex.h"	/* texutre pattern */
static void init_prim(DB *db)
{
	u_short	clut[32];	/* CLUT entry */
	SPRT_16	*sp;		/* work */
	int	i;		/* work */

	/* set bg color */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* load texture pattern */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifdef DEBUG
	DumpTPage(db->draw.tpage);
#endif
	/* load texture CLUT */
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
#ifdef DEBUG
		DumpClut(clut[i]);
#endif
	}

	/* initialize sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		/* set SPRT_16 */
		SetSprt16(sp);		
		
		/* semi-ambient is OFF */
		SetSemiTrans(sp, 0);	
		
		/* shaded texture is OFF */
		SetShadeTex(sp, 1);	
		
		/* texture point is (0,0) */
		setUV0(sp, 0, 0);	
		
		/* set CLUT */
		sp->clut = clut[i%32];	
	}
}

/*
 * preset unchanged primitve members */
static init_point(POS *pos)	
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* starting coordinate X */
		pos->y  = rand();		/* starting coordinate Y */
		pos->dx = (rand() % 4) + 1;	/* displacement distance X (1<=x<=4)  */
		pos->dy = (rand() % 4) + 1;	/* displacement distance Y (1<=y<=4)  */
		pos++;
	}
}

/*
 * parse controller */
static int pad_read(int	n)		
{
	u_long	padd;

	/* get controller data */
	padd = PadRead(1);

	if(padd & PADLup)	n += 4;		/* incriment */
	if(padd & PADLdown)	n -= 4;		/* decriment */
	if(padd & PADRup)	n += 4;		/* incriment */
	if(padd & PADRdown)	n -= 4;		/* decriment */
	if(padd & PADselect) 	return(-1);	/* terminate */

	/* clamp between [0,MAXOBJ-1]; see libgpu.h */
	limitRange(n, 1, MAXOBJ-1);	
					
	return(n);
}

/*
 * callback */
static void cbvsync(void)
{
	/* print absolute VSync count*/
	FntPrint("V-BLNK(%d)\n", VSync(-1));	
}


