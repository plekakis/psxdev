/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*****************************************************************
 * -*- c -*-
 * $RCSfile: main.c,v $
 *
 * Copyright (C) 1995 by Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/

#ifdef RCSID
#ifndef lint
static char rcsid [] = "$Id: main.c,v 1.12 1996/09/06 07:47:26 kaol Exp $ : \
	Copyright (C) by 1995 Sony Computer Entertainment Inc.";
#endif
#endif /* RCSID */

#include <sys/types.h>
#include <stdio.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h> 
#include <libcd.h> 
#ifdef __psx__
#include <libsn.h> 
#else
#define pollhost()
#endif

#ifndef NULL
#define NULL 0
#endif

/* ----------------------------------------------------------------
 * defines for graphics
 * ---------------------------------------------------------------- */

#define WIDTH		320		/* screen width */
#define HEIGHT		240		/* screen height */
#define CENTER		(WIDTH/2)	/* screen center */
#define OTSIZE		16		/* ot size */
#define FTSIZE		64		/* number of font sprites */
#define BARNUM		2		/* amount of bar */
#define TSIZE		10		/* peak thumb width */
#define BSIZE		128		/* bar size / 2 */
#define YS		100		/* Top    Y coordinates of L bar */
#define YE		((YS)+5)	/* Bottom Y coordinates of L bar */
#define YW		40		/* Line skip */
#define MAXBAR		(CENTER+BSIZE+TSIZE) /* bar right */
#define MINBAR		CENTER-BSIZE	/* bar left */

typedef struct {
    DRAWENV	draw;			/* drawing environment */
    DISPENV	disp;			/* display environment */
    int 	id;			/* font id */
    u_long	ot [OTSIZE];		/* ordering table */
    POLY_F4	bar     [BARNUM];	/* bar polygon */
    POLY_F4	current [BARNUM];	/* current max. polygon */
    POLY_F4	peak    [BARNUM];	/* peak max. polygon */
} DB;

DB db [2];			/* double buffer */
void init_prim (void);		/* Initialize of primitives */

/* ----------------------------------------------------------------
 *	for SOUND
 * ---------------------------------------------------------------- */
long addr;			/* SPU IRQ addr. */
SpuDecodeData d_data;		/* SPU decode data buffer */

/*
 *	IRQ callback
 */
SpuIRQCallbackProc
eachIRQ (void)
{
    SpuSetIRQ (SPU_OFF); /**/

    SpuReadDecodeData (&d_data, SPU_CDONLY); /**/

    return;
}

/*
 *	DMA Transfer callback
 */
SpuTransferCallbackProc
eachDMA (void)
{
    if (addr == 0x0)
	addr = 0x200;
    else
	addr = 0x0;

    /* IRQ address chenge */
    SpuSetIRQAddr (addr); /**/

    SpuSetIRQ (SPU_ON); /**/

    return;
}

unsigned long left,  right;		/* CD volume: MAX value */
unsigned long left_, right_;		/* CD volume: PEAK value */

#define LEFT 0
#define RIGHT 1

/* ----------------------------------------------------------------
 *	Main routine
 * ---------------------------------------------------------------- */

main (void)
{
    /* common */
    long i;
    long l_, r_, l__, r__;
    extern void print_data (int);

    /* for graphics */
    static RECT	bg = {0, 0, 640, 480};
    DB *cdb = db;			/* current double buffer */

    /* for sound */
    SpuCommonAttr c_attr;
    extern void max_volume (void);
#ifdef FOR_MENU
    CdlLOC loc[100];		/* Play from #2 track */
    int ntoc;
    unsigned char param [4], result [8];
#endif /* FOR_MENU */

    /* ----------------------------------------------------------------
     *		äÑÇËçûÇ›ä¬ã´ÇÃèâä˙âª /
     *		Initialize interrupt environment.
     * ---------------------------------------------------------------- */
    ResetCallback();

    ResetGraph (0);		/* reset graphic subsystem */
    CdInit ();			/* reset CD env. */
    SpuInit ();			/* reset SPU env. */
    PadInit (0);		/* reset PAD */
    SetGraphDebug (0);

    /* ----------------------------------------------------------------
     *	GRAPHICS: setting
     * ---------------------------------------------------------------- */

    /* initialize environment for double buffer */
    SetDefDrawEnv (&db[0].draw, 0,      0, WIDTH, HEIGHT);
    SetDefDrawEnv (&db[1].draw, 0, HEIGHT, WIDTH, HEIGHT);
    SetDefDispEnv (&db[0].disp, 0, HEIGHT, WIDTH, HEIGHT);
    SetDefDispEnv (&db[1].disp, 0,      0, WIDTH, HEIGHT);

    /* intialize font for screen print */
    FntLoad (640, 0);
    db[0].id = FntOpen (MINBAR, YS - 10, 200, 150, 0, FTSIZE);
    db[1].id = FntOpen (MINBAR, YS - 10, 200, 150, 0, FTSIZE);

    /* initialize primitive buffer */
    init_prim ();

    /* display */
    ClearImage (&bg, 0, 0, 0);
    SetDispMask (1);		/* enable to display */

    /* ----------------------------------------------------------------
     *	SOUND: setting
     * ---------------------------------------------------------------- */

    left  = right  = 0;		/* current max */
    left_ = right_ = 0;		/* peak level */

    /* Clearing data area */
    for (i = 0; i < SPU_DECODEDATA_SIZE; i ++) {
	d_data.cd_left  [i] = 0;
	d_data.cd_right [i] = 0;
    }

    /* Main volume, CD volume */
    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR |
		   SPU_COMMON_CDVOLL |
		   SPU_COMMON_CDVOLR |
		   SPU_COMMON_CDMIX
		   );
    c_attr.mvol.left       = 0x3fff;
    c_attr.mvol.right      = 0x3fff;
    c_attr.cd.volume.left  = 0x7fff;
    c_attr.cd.volume.right = 0x7fff;
    c_attr.cd.mix	   = SPU_ON;

    SpuSetCommonAttr (&c_attr);

    /* ----------------------------------------------------------------
     *	SOUND: DMA Transfer setting
     * ---------------------------------------------------------------- */

    /* Transfer mode for main memory <-> sound-buffer */
    SpuSetTransferMode (SPU_TRANSFER_BY_DMA);

    /* Transfer callback */
    (void) SpuSetTransferCallback ((SpuTransferCallbackProc) eachDMA); /**/

    /* ----------------------------------------------------------------
     *	SOUND: IRQ setting
     * ---------------------------------------------------------------- */

    /* IRQ callback  */
    SpuSetIRQCallback ((SpuIRQCallbackProc) eachIRQ); /**/

    addr = 0x200;

    /* IRQ address */
    SpuSetIRQAddr (addr); /**/

#ifdef FOR_MENU
    /* ----------------------------------------------------------------
     * This part is stolen from sample/cd/tuto1.c
     * ---------------------------------------------------------------- */
    /*
     * If defined FOR_MENU, plays CD from #2 track.
     */
    while ((ntoc = CdGetToc(loc)) == 0) { 		/* Read TOC */
	printf("No TOC found: please use CD-DA disc...\n");
    }
	
/* #define TOCBUG */
#ifdef TOCBUG
    /* Ç‡ÇµÇ‡ÅATOC ÇÃèÓïÒÇ™Ç∏ÇÍÇƒÇ¢ÇΩÇÁÇ±Ç±Ç≈ï‚ê≥ÇµÇƒÇµÇ‹Ç§
     * :if the information of TOC gets out of position, correct. */
    for (i = 1; i < ntoc; i++) {
	CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]);
    }
#endif
    /* 2 ã»ñ⁄Çââët :play 2nd music */
    param [0] = CdlModeRept|CdlModeDA;	/* report ON / CD-DA ON */
    CdControlB (CdlSetmode, param, 0);	/* set mode */
    VSync (3);				/* wait three vsync times */
    CdControlB (CdlPlay, (unsigned char *)&loc [2], 0);	/* play */
#else  /* FOR_MENU */
#define _CDPLAY()       CdControl(CdlPlay, 0, 0)
    _CDPLAY ();
#endif /* FOR_MENU */

    /* IRQ reset */
    SpuSetIRQ (SPU_ON); /**/

    while (1) {

	cdb = (cdb == db) ? db + 1 : db;	/* swap double buffer */
	ClearOTag (cdb->ot, OTSIZE);		/* clear ordering table */

	/* Move peak thumb & level meter */
	l_  = (left   * 256) / 0x8000 + MINBAR;
	r_  = (right  * 256) / 0x8000 + MINBAR;
	l__ = (left_  * 256) / 0x8000 + MINBAR;
	r__ = (right_ * 256) / 0x8000 + MINBAR;
	setXY4 (&cdb->current [LEFT],
		MINBAR,     YS,
		l_ + TSIZE, YS,
		MINBAR,     YE,
		l_ + TSIZE, YE);
	setXY4 (&cdb->current [RIGHT],
		MINBAR,     YS + YW,
		r_ + TSIZE, YS + YW,
		MINBAR,     YE + YW,
		r_ + TSIZE, YE + YW);
	setXY4 (&cdb->peak [LEFT],
		l__,         YS,
		l__ + TSIZE, YS,
		l__,         YE,
		l__ + TSIZE, YE);
	setXY4 (&cdb->peak [RIGHT],
		r__,         YS + YW,
		r__ + TSIZE, YS + YW,
		r__,         YE + YW,
		r__ + TSIZE, YE + YW);

	/* append to ordering table */
	for (i = 0; i < BARNUM; i++) {
	    AddPrim (cdb->ot,     &cdb->bar     [i]);
	    AddPrim (cdb->ot + 1, &cdb->current [i]);
	    AddPrim (cdb->ot + 2, &cdb->peak    [i]);
	}

	print_data (cdb->id);	/* print data */

	DrawSync(0);		/* wait for end of drawing */
#if 0
	pollhost ();
#endif
	VSync (0);

	PutDispEnv (&cdb->disp); /* update display environment */
	PutDrawEnv (&cdb->draw); /* update drawing environment */
	DrawOTag (cdb->ot);

	/* Quit: pad assign */
	if (PadRead (0) & PADk) {
	    break;
	}

	/* calculate max volume & peak level */
	max_volume ();
    }

    /* IRQ reset */
    SpuSetIRQ (SPU_OFF); /**/

    /* Clear IRQ callback  */
    SpuSetIRQCallback ((SpuIRQCallbackProc) NULL); /**/

    /* Clear Transfer callback */
    (void) SpuSetTransferCallback ((SpuTransferCallbackProc) NULL);

    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR |
		   SPU_COMMON_CDVOLL |
		   SPU_COMMON_CDVOLR |
		   SPU_COMMON_CDMIX
		   );
    c_attr.mvol.left       = 0;
    c_attr.mvol.right      = 0;
    c_attr.cd.volume.left  = 0;
    c_attr.cd.volume.right = 0;
    c_attr.cd.mix	   = SPU_OFF;

    SpuSetCommonAttr (&c_attr);

    CdStop ();			/* CD stop */
    PadStop ();			/* pad stop */
    SpuQuit ();			/* SPU process end */

    ResetGraph (3);		/* reset graphic subsystem */
    StopCallback ();		/* stop callback processing */

    return;
}

/* ----------------------------------------------------------------
 *	GRAPHICS: initialize drawing primitives
 * ---------------------------------------------------------------- */

void
init_prim (void)
{
    int i, j;	/* work */

    for (j = 0; j < 2; j++) {

	/* Background */
	db[j].draw.isbg = 1;
	setRGB0 (&(db [j].draw),
		 /* Sky Blue */
		 0x40,		/* R */
		 0xa0,		/* G */
		 0xc0);		/* B */
	
	for (i = 0; i < BARNUM; i++) {

	    /* intialize bar */
	    SetPolyF4 (&db [j].bar [i]);
	    setRGB0 (&db [j].bar [i],
		     /* Blue */
		     0x0,	/* R */
		     0x0,	/* G */
		     0xff);	/* B */
	    setXY4 (&db [j].bar [i],
		    MINBAR, YS + i * YW, /* NW */
		    MAXBAR, YS + i * YW, /* NE */
		    MINBAR, YE + i * YW, /* SW */
		    MAXBAR, YE + i * YW); /* SE */

	    /* intialize current max. */
	    SetPolyF4 (&db [j].current [i]);
	    setRGB0 (&db [j].current [i],
		     /* White */
		     0xff,	/* R */
		     0xff,	/* G */
		     0xff);	/* B */
	    setXY4 (&db [j].current [i],
		    MINBAR,         YS + i * YW,
		    MINBAR + TSIZE, YS + i * YW,
		    MINBAR,         YE + i * YW,
		    MINBAR + TSIZE, YE + i * YW);

	    /* intialize peak */
	    SetPolyF4 (&db [j].peak [i]);
	    setRGB0 (&db [j].peak [i],
		     /* Red */
		     0xff,	/* R */
		     0x0,	/* G */
		     0x0);	/* B */
	    setXY4 (&db [j].peak [i],
		    MINBAR,         YS + i * YW,
		    MINBAR + TSIZE, YS + i * YW,
		    MINBAR,         YE + i * YW,
		    MINBAR + TSIZE, YE + i * YW);
	}
    }
}

/* ----------------------------------------------------------------
 *	GRAPHICS: 
 * ---------------------------------------------------------------- */

/*
 * Print bar name 
 */
void
print_data (int id)
{
    FntPrint (id, "L: %04x peak/%04x\n", left, left_);
    FntPrint (id, "\n\n\n\n");
    FntPrint (id, "R: %04x peak/%04x\n", right, right_);
    FntFlush (id);		/* print on screen */
}

/* ----------------------------------------------------------------
 *	SOUND: Search maximum value of CD 16bit straight PCM
 * ---------------------------------------------------------------- */

#define ABS(x) (((x)<0)?(-(x)):(x))

void
max_volume (void)
{
    long min_, max_;
    short retL = 0, l_;
    short retR = 0, r_;
    long i;

    static long timesL = 0, timesR = 0;

    if (addr == 0x0) {
	/* 1st part is available */
	min_ = 0x0;
	max_ = 0x100;
    } else {
	/* 2nd part is available */
	min_ = 0x100;
	max_ = 0x200;
    }

    for (i = min_; i < max_; i ++) {
	l_ = ABS(d_data.cd_left [i]);
	r_ = ABS(d_data.cd_right [i]);
	if (retL < l_) {
	    retL = l_;
	}
	if (retR < r_) {
	    retR = r_;
	}
    }
    left  = (long) retL;
    right = (long) retR;

    /* peak level */
    if (left_ < left) {
	left_ = left;
	timesL = 0;
    }
    if (right_ < right) {
	right_ = right;
	timesR = 0;
    }

    /* peak thumb: hold 2s. */
    if (timesL < 120) {
	timesL ++;
    } else {
	timesL = 0;
	left_ = left;
    }
    if (timesR < 120) {
	timesR ++;
    } else {
	timesR = 0;
	right_ = right;
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
