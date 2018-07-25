/*
 * $PSLibId: Runtime Library Versin 3.0$
 */
/*                      balls: simplest sample
 *
 *                 グラフィックのバックグラウンド
 *
 *      libcd の速度をチェックするために、毎フレーム2000個の balls を
 *      表示する。毎 VSync ごとにコールすればよい。
 *                      
 *      Copyright (C) 1994 by Sony Computer Entertainment
 *                      All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Primitive Buffer
 */
#define OTSIZE          2                       /* size of ordering table */
#define MAXOBJ          2000                    /* max sprite number */
typedef struct {                
	DRAWENV         draw;                   /* drawing environment */
	DISPENV         disp;                   /* display environment */
	u_long          ot[OTSIZE];             /* ordering table */
	SPRT_16         sprt[MAXOBJ];           /* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {                
	u_short x, y;                   /* current point */
	u_short dx, dy;                 /* verocity */
} POS;

/*
 * Limitations
 */
#define FRAME_X         320             /* frame size (320x240) */
#define FRAME_Y         240
#define WALL_X          (FRAME_X-16)    /* reflection point */
#define WALL_Y          (FRAME_Y-16)

static int init_prim();
static int init_point();

balls()
{
	static first = 1;       
	static POS      pos[MAXOBJ];
	static DB       db[2];                  /* double buffer */
	static DB       *cdb;                   /* current double buffer */
	static int      nobj = MAXOBJ;          /* object number */
	static u_long   *ot;                    /* current OT */
	static SPRT_16  *sp;                    /* work */
	static POS      *pp;                    /* work */
	static int      i, x, y;                /* work */
	
	if (first) {
		first = 0;
		
		/* initialize environment for double buffer */
		SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
		SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
		SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
		SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
		init_prim(&db[0]);      /* initialize primitive buffers #0 */
		init_prim(&db[1]);      /* initialize primitive buffers #1 */
		init_point(pos);        /* set initial geometries */

		/* display */
		SetDispMask(1);         
	}
	cdb  = (cdb==db)? db+1: db;     /* swap double buffer ID */

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
			
		setXY0(sp, x, y);       /* update vertex */
		AddPrim(ot, sp);        /* apend to OT */
	}
	DrawSync(0);            /* wait for end of drawing */
	VSync(0);               /* wait for V-BLNK */
	PutDispEnv(&cdb->disp); /* update display environment */
	PutDrawEnv(&cdb->draw); /* update drawing environment */
	DrawOTag(cdb->ot);
}


/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

static init_prim(db)
DB      *db;
{
	u_short clut[32];               /* CLUT entry */
	SPRT_16 *sp;                    /* work */
	int     i;                      /* work */
	
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* load texture pattern and CLUT */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	
	for (i = 0; i < 32; i++) 
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	
	/* init sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);                  /* set SPRT_16 primitve ID */
		SetSemiTrans(sp, 0);            /* semi-amibient is OFF */
		setUV0(sp, 0, 0);               /* texture point is (0,0) */
		setRGB0(sp, 0x40, 0x40, 0x40);  /* half color */
		sp->clut = clut[i%32];          /* set CLUT */
	}
}       

/*
 * Initialize sprite position and verocity
 */
static init_point(pos)
POS     *pos;
{
	int     i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();
		pos->y  = rand();
		pos->dx = (rand()&0x01)+1;
		pos->dy = (rand()&0x01)+1;
		pos++;
	}
}

