/* $PSLibId: Runtime Library Release 3.6$ */
/*
 * File:main8.c
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <sys/file.h>

/* position limit of balls */
#define XMAX 320-16
#define YMAX 240-16
#define XMIN 0
#define YMIN 0

/* version */
char *version = "Ver. 1.0  950508";

/*
 * Primitive Buffer
 */
#define OTSIZE		16			/* size of ordering table */
#define NUM_SPRT	2
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	SPRT_16		sprt[NUM_SPRT];	/* 16x16 fixed-size sprite */
} DB;

DB	db[2];		/* ダブルバッファ */
DB	*cdb;		/* 現在のバッファ */

/* プロトタイプ宣言 */
static void init_prim(DB *db);		/* primitives */
static int  pad_read(int chan, int *x, int *y);	/* controller asscee */
static void cbvsync(void);		/* VSync callback function */


/* 
 * main
*/
main()
{	
	u_long	*ot;
	int	i;
	int dx[2], x[2], y[2];	

        ResetCallback();
	/* controller driver */
	_init_cont();
	_start_remote();
		
	/* graphic system */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	VSyncCallback(cbvsync);	/* set callback */
		
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* font system */
	FntLoad(960, 256);		/* load basic font pattern */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	/* balls */
	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */
	x[0] = x[1] = (XMAX-XMIN)/2;
	y[0] = y[1] = (YMAX-YMIN)/2;
	y[1] -= 20; 
	dx[0] = 2; dx[1] = -2;

	/* remove display mask */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	/* main loop */
	while (pad_read(0,&x[0],&y[0])!=-1) {	/* assecc to the comtrollers */
	    pad_read(2,&x[1],&y[1]);
		_do_remote();	
#if 1
		/* exchange the double buffers */
		cdb  = (cdb==db)? db+1: db;

		/* spped of balls */
		for(i=0;i<2;i++) {
			if((x[i]+=dx[i])>XMAX) {
				x[i]=XMAX; dx[i] *= -1;
			}
			else if(x[i]<XMIN) {
				x[i]=XMIN; dx[i] *= -1;
			}
		}
		
 		/* clear the ordering table */
		ClearOTag(cdb->ot, OTSIZE);	

		/* update the sprites */
		for(i=0;i<2;i++) {
			setXY0(&cdb->sprt[i], x[i], y[i]);	/* update vertex */
			AddPrim(cdb->ot, &cdb->sprt[i]);	/* apend to OT */
		}
		
		/* dispaly */
		DrawSync(0);		/* wait for end of drawing */
		VSync(0);		/* wait for V-BLNK (1/60) */
		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		DrawOTag(cdb->ot);

		/* title message */
		FntPrint("\n    REMOTE CONTROLLER\n");

		/* position of balls */
		for(i=0;i<2;i++) {
			FntPrint("pos(0) y = %03d", y[i]);
			FntPrint("    x = %03d\n", x[i]);
		}
		FntFlush(-1);
#endif
	}
	/* exit operation */
	PadStop();
	return(0);
}

/*
 * sprites
*/
#include "balltex.h"	/* texture for balls */

static void init_prim(DB *db)
{
	u_short	clut[32];		/* CLUT entry */
	int	i;			/* work */
	
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);
	
	/* load texture pattern and CLUT */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	/*DumpTPage(db->draw.tpage);*/
	
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
		/*DumpClut(clut[i]);*/
	}
	
	/* init sprite */
	for(i=0;i<2;i++) {
		SetSprt16(&db->sprt[i]);			/* set SPT_16 primitve ID */
		SetSemiTrans(&db->sprt[i], 0);		/* semi-amibient is OFF */
		SetShadeTex(&db->sprt[i], 1);		/* shading&texture is OFF */
		setUV0(&db->sprt[i], 0, 0);		/* texture point is (0,0) */
		(db->sprt[i]).clut = clut[i%32];		/* set CLUT */
	}
}	

/*
 * assecc to the controllers
 * returns -1 when SELECT is pushed down
*/
static
int pad_read(int chan, int *x, int *y)
{
	u_long	padd = _get_cont(chan);	/* get the status of a controller */

	if((padd & PADLdown)==0)	{ if((*y)<YMAX) (*y)++; }
	if((padd & PADLup)==0)	{ if((*y)>YMIN) (*y)--; }
	if((padd & PADLright)==0)	{ if((*x)<XMAX) (*x)++; }
	if((padd & PADLleft)==0)	{ if((*x)>XMIN) (*x)--; }


	FntPrint("padd(%d)=%08x\n",chan,padd);
			
	if(chan<2) {
		/* SELECT ? */
		if((padd & PADk)==0) 	return(-1);
	}
	return(0);
}		


/*
 * VSync callback 
*/
static void cbvsync(void)
{
	/* empty */
	/*	FntPrint("V-BLNK(%d)\n", VSync(-1));	*/
}

