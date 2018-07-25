/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			balls: sample program
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Aug,31,1993	suzu
 *	2.00		Nov,17,1993	suzu	(using 'libgpu)
 *	3.00		Dec.27.1993	suzu	(rewrite)
 *	3.01		Dec.27.1993	suzu	(for newpad)
 *      3.02            Aug.31.1994     noda    (for KANJI)
 *      3.03            Mar.28.1997     sachiko (for AutoPaD)
 */

/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto1.c,v $
 *
 * Copyright (C) 1995 by Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/

/*
 * SPU streaming library sample:
 *
 * 	The parts coming under SPU streaming library are the regions
 *	between <SPU-STREAMING> and </SPU-STREAMING>.
 */

#ifdef RCSID
#ifndef lint
static char rcsid [] = "$Id: tuto1.c,v 1.8 1997/05/02 13:07:49 ayako Exp $ : \
	Copyright (C) by 1995 Sony Computer Entertainment Inc.";
#endif
#endif /* RCSID */

/*
 * Sound and SPU streaming
 */
#define SOUND /**/				/* sound available */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Kanji Printf
 */
/* #define KANJI /**/

/*
 * Primitive Buffer
 */
#define OTSIZE		16			/* size of ordering table */
#define MAXOBJ		4000			/* max sprite number */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	SPRT_16		sprt[MAXOBJ];		/* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {		
	u_short x, y;			/* current point */
	u_short dx, dy;			/* verocity */
} POS;

/*
 * Limitations
 */
#define	FRAME_X		320		/* frame size (320x240) */
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* reflection point */
#define WALL_Y		(FRAME_Y-16)

static void init_prim(DB *db);	/* preset unchanged primitive members */
static int  pad_read(int n);	/* parse controller */
static void cbvsync(void);	/* callback for VSync */
static int  init_point(POS *pos);/* initialize position table */

#ifdef SOUND /* ------------------------------------------------ */

#include <libspu.h>

/*
 * If you want to run with using 14 voices, define USE_14VOICES.
 */
#define USE_14VOICES /**/
/* #define USE_8VOICES /**/
/*
 * If you want to check with sound when all streams are finished, define USE_CONFIRM
 */
/* #define USE_CONFIRM /**/

/*
 * for debug printing
 */
/* #define DEBUG /**/

/* <PC-MENU> */
/* Limitation environment for PC-MENU */
#if defined(FOR_MENU) 
#ifdef USE_14VOICES
#undef USE_14VOICES
#endif
#ifdef USE_8VOICES
#undef USE_8VOICES
#endif
#ifdef USE_CONFIRM
#undef USE_CONFIRM
#ifdef DEBUG
#undef DEBUG
#endif
#endif
#define USE_8VOICES /* because of memory limitation */
#endif

/* </PC-MENU> */

#ifdef DEBUG
#define PRINTF(x) (void) printf x
#else
#define PRINTF(x)
#endif

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

#ifdef USE_CONFIRM
unsigned char sin_wave [] = {
#include "sin.h"
};
#endif /* USE_CONFIRM */

#define SIN_DATA_SIZE (0x10 * 10)

/* <SPU-STREAMING> */
#define SPU_BUFSIZE     0x4000
#define SPU_BUFSIZEHALF 0x2000
#define DATA_SIZE       0x7a000 /* 499712 /**/

#define TOP_ADDR 0x800a0000
unsigned long data_start_addr [] = {
    TOP_ADDR,			/* 0L */
    TOP_ADDR + DATA_SIZE,	/* 0R */
#if defined(USE_8VOICES) || defined(USE_14VOICES)
    TOP_ADDR + DATA_SIZE * 2,	/* 1L */
    TOP_ADDR + DATA_SIZE * 3,	/* 1R */
    TOP_ADDR + DATA_SIZE * 4,	/* 2L */
    TOP_ADDR + DATA_SIZE * 5,	/* 2R */
    TOP_ADDR + DATA_SIZE * 6,	/* 3L */
    TOP_ADDR + DATA_SIZE * 7,	/* 3R */
#ifdef USE_14VOICES
    TOP_ADDR + DATA_SIZE * 8,	/* 4L */
    TOP_ADDR + DATA_SIZE * 9,	/* 4R */
    TOP_ADDR + DATA_SIZE * 10,	/* 5L */
    TOP_ADDR + DATA_SIZE * 11,	/* 5R */
    TOP_ADDR + DATA_SIZE * 12,	/* 6L */
    TOP_ADDR + DATA_SIZE * 13	/* 6R */
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */
};

#define L0      0
#define R0      1
#define L1      2
#define R1      3
#define L2      4
#define R2      5
#define L3      6
#define R3      7
#define L4      8
#define R4      9
#define L5     10
#define R5     11
#define L6     12
#define R6     13
#ifdef USE_8VOICES
#define VOICE_LIMIT 8
#else
#ifdef USE_14VOICES
#define VOICE_LIMIT 14
#else
#define VOICE_LIMIT 2
#endif /* USE_14VOICES */
#endif /* USE_8VOICES */

#ifdef USE_CONFIRM
#define CONFIRM		VOICE_LIMIT
#endif /* USE_CONFIRM */

unsigned long played_size;	/* the size finished to set */
SpuStEnv *st;

unsigned long st_stop;
#define ST_STOP_INIT_VAL 0

unsigned long st_stat = 0;
long st_load_ave = 0;

unsigned long all_voices = (SPU_VOICECH (L0) | SPU_VOICECH (R0)
#if defined(USE_8VOICES) || defined(USE_14VOICES)
			    | SPU_VOICECH (L1) | SPU_VOICECH (R1)
			    | SPU_VOICECH (L2) | SPU_VOICECH (R2)
			    | SPU_VOICECH (L3) | SPU_VOICECH (R3)
#ifdef USE_14VOICES
			    | SPU_VOICECH (L4) | SPU_VOICECH (R4)
			    | SPU_VOICECH (L5) | SPU_VOICECH (R5)
			    | SPU_VOICECH (L6) | SPU_VOICECH (R6)
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */
			    );
/* </SPU-STREAMING> */

#ifdef USE_CONFIRM
#define SPU_MALLOC_MAX		(VOICE_LIMIT+1)
#else  /* USE_CONFIRM */
#define SPU_MALLOC_MAX		VOICE_LIMIT
#endif /* USE_CONFIRM */

char spu_malloc_rec [SPU_MALLOC_RECSIZ * (SPU_MALLOC_MAX + 1)];

/* <SPU-STREAMING> */
void
spustCB_next (unsigned long voice_bit)
{
    register long i;

    if (played_size <= (DATA_SIZE - SPU_BUFSIZEHALF)) {
	played_size += SPU_BUFSIZEHALF;
	for (i = 0; i < VOICE_LIMIT; i ++) {
	    st->voice [i].data_addr += SPU_BUFSIZEHALF;
	}
    } else {
	/* return to TOP */
	for (i = 0; i < VOICE_LIMIT; i ++) {
	    st->voice [i].data_addr = data_start_addr [i];
	}
	played_size = SPU_BUFSIZEHALF;
    }

    if (st_stop != ST_STOP_INIT_VAL) {
	for (i = 0; i < VOICE_LIMIT; i += 2) {
	    if (st_stop & SPU_VOICECH (i)) {
		/* L-ch */
		st->voice [i].status        = SPU_ST_STOP;
		st->voice [i].last_size     = SPU_BUFSIZEHALF;
		/* R-ch */
		st->voice [i + 1].status    = SPU_ST_STOP;
		st->voice [i + 1].last_size = SPU_BUFSIZEHALF;
	    }
	}
	st_stat &= ~st_stop;
	st_stop = ST_STOP_INIT_VAL;
    }
}

SpuStCallbackProc
spustCB_preparation_finished (unsigned long voice_bit, long p_status)
{
    /* p_status: SPU_ST_PREPARE ... when status is SPU_ST_PREPARE
     		 SPU_ST_PLAY    ... when status is SPU_ST_PLAY */

    PRINTF (("preparation: [%06x] [%s]\n",
	     voice_bit,
	     p_status == SPU_ST_PREPARE ? "PREPARE" : "PLAY"
	     ));

    if (p_status == SPU_ST_PREPARE) {
	spustCB_next (voice_bit);
    }

    if (st_stat != 0L) {
	SpuStTransfer (SPU_ST_START, voice_bit);
    }
}

/*ARGSUSED*/
SpuStCallbackProc
spustCB_transfer_finished (unsigned long voice_bit, long t_status)
{
    /* Always t_status is SPU_ST_PLAY */

    spustCB_next (voice_bit);
}

/*ARGSUSED*/
SpuStCallbackProc
spustCB_stream_finished (unsigned long voice_bit, long s_status)
{
    /* s_status: SPU_ST_PLAY  ... when status is SPU_ST_PLAY
     		 SPU_ST_FINAL ... when status is SPU_ST_FINAL */

    PRINTF (("stop: [%06x] [%s]\n",
	     voice_bit,
	     s_status == SPU_ST_PLAY ? "PLAY" : "FINAL"
	     ));

    SpuSetKey (SPU_OFF, voice_bit);
#ifdef USE_CONFIRM
    SpuSetKey (SPU_ON, SPU_VOICECH(CONFIRM));
#endif /* USE_CONFIRM */
}

/* </SPU-STREAMING> */

#endif /* SOUND ------------------------------------------------ */

main()
{
	POS	pos[MAXOBJ];
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current double buffer */
	/* char	s[128];		/* strings to print */
	int	nobj = 1;	/* object number */
	u_long	*ot;		/* current OT */
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
#ifdef SOUND
	SpuVoiceAttr s_attr;
	SpuCommonAttr c_attr;
	/* <SPU-STREAMING> */
	unsigned long buffer_addr [SPU_MALLOC_MAX];
	long status;
	/* </SPU-STREAMING> */
#endif /* SOUND */

	/* ----------------------------------------------------------------
	 *		Initialize interrupt environment. */

	ResetCallback();

	PadInit(0);		/* reset PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	VSyncCallback(cbvsync);	/* set callback */
		
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init font environment */
#ifdef KANJI	/* KANJI */	
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	FntLoad(960, 256);		/* load basic font pattern */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */
	init_point(pos);		/* set initial geometries */

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

#ifdef SOUND
	/* ----------------------------------------------------------------
	 *		SPU Initialize
	 * ---------------------------------------------------------------- */

	SpuInit ();
	SpuInitMalloc (SPU_MALLOC_MAX, spu_malloc_rec);

	/* ----------------------------------------------------------------
	 *		SPU Common attributes
	 * ---------------------------------------------------------------- */

	c_attr.mask = (SPU_COMMON_MVOLL |
		       SPU_COMMON_MVOLR);

	c_attr.mvol.left  = 0x3fff;	/* Master volume (left) */
	c_attr.mvol.right = 0x3fff;	/* Master volume (right) */

	SpuSetCommonAttr (&c_attr);

	/* ----------------------------------------------------------------
	 *		SPU Transfer Mode
	 * ---------------------------------------------------------------- */

	SpuSetTransferMode (SPU_TRANSFER_BY_DMA); /* transfer by DMA */

	/* <SPU-STREAMING> */
	/* ----------------------------------------------------------------
	 *		SPU streaming setting
	 * ---------------------------------------------------------------- */

	/*		Initialize SPU streaming                            */

	st = SpuStInit (0);

	/*		Set some callback functions                         */

	/* For finishing SPU streaming preparation */
	(void) SpuStSetPreparationFinishedCallback ((SpuStCallbackProc)
						    spustCB_preparation_finished);
	/* For next transferring */
	(void) SpuStSetTransferFinishedCallback ((SpuStCallbackProc)
						 spustCB_transfer_finished);
	/* For finising SPU streaming with some voices */
	(void) SpuStSetStreamFinishedCallback ((SpuStCallbackProc)
					       spustCB_stream_finished);

	/*		Allocate buffers in sound buffer                    */

	/* for SPU streaming itself */
	for (i = 0; i < VOICE_LIMIT; i ++) {
	    if ((buffer_addr [i] = SpuMalloc (SPU_BUFSIZE)) == -1) {
		printf ("SpuMalloc : %d\n", i);
		return 0;		/* ERROR */
	    }
	}

#ifdef USE_CONFIRM
	/* for the sound when finish SPU streaming */
	if ((buffer_addr [CONFIRM] = SpuMalloc (SIN_DATA_SIZE)) == -1) {
	    printf ("SpuMalloc : %d\n", CONFIRM);
	    return 0;		/* ERROR */
	}

	/* Transfer data for confirming KEY OFF */
	(void) SpuSetTransferStartAddr (buffer_addr [CONFIRM]);
	(void) SpuWrite (sin_wave, SIN_DATA_SIZE);
	(void) SpuIsTransferCompleted (SPU_TRANSFER_WAIT);
#endif /* USE_CONFIRM */

	/* Set SPU streaming environment */

	/* Size of each buffer in sound buffer */
	st->size = SPU_BUFSIZE;

	/* Top address of each buffer in sound buffer */
	for (i = 0; i < VOICE_LIMIT + 1; i ++) {
	    st->voice [i].buf_addr = buffer_addr [i];
	}
	/* </SPU-STREAMING> */

	/* ----------------------------------------------------------------
	 *		Voice attributes
	 * ---------------------------------------------------------------- */

	/* attribute masks */
	s_attr.mask = (SPU_VOICE_VOLL |
		       SPU_VOICE_VOLR |
		       SPU_VOICE_PITCH |
		       SPU_VOICE_WDSA |
		       SPU_VOICE_ADSR_AMODE |
		       SPU_VOICE_ADSR_SMODE |
		       SPU_VOICE_ADSR_RMODE |
		       SPU_VOICE_ADSR_AR |
		       SPU_VOICE_ADSR_DR |
		       SPU_VOICE_ADSR_SR |
		       SPU_VOICE_ADSR_RR |
		       SPU_VOICE_ADSR_SL
		       );
	
	/* attribute values: L-ch */
	s_attr.volume.left  = 0x3fff;
	s_attr.volume.right = 0x0;
	s_attr.pitch        = 0x1000;
	s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	s_attr.ar           = 0x0;
	s_attr.dr           = 0x0;
	s_attr.sr           = 0x0;
	s_attr.rr           = 0x3;
	s_attr.sl           = 0xf;
	
	for (i = L0; i < VOICE_LIMIT; i += 2) {
	    s_attr.voice = SPU_VOICECH (i);
	    s_attr.addr  = buffer_addr [i];
	    SpuSetVoiceAttr (&s_attr);
	}
	
	/* attribute values: R-ch */
	s_attr.volume.left  = 0x0;
	s_attr.volume.right = 0x3fff;
	for (i = R0; i < VOICE_LIMIT; i += 2) {
	    s_attr.voice = SPU_VOICECH (i);
	    s_attr.addr  = buffer_addr [i];
	    SpuSetVoiceAttr (&s_attr);
	}
	
#ifdef USE_CONFIRM
	s_attr.voice = SPU_VOICECH (CONFIRM);
	s_attr.volume.left  = 0x3fff;
	s_attr.volume.right = 0x3fff;
	s_attr.addr         = buffer_addr [CONFIRM];
	
	/* set : CONFIRM */
	SpuSetVoiceAttr (&s_attr);
#endif /* USE_CONFIRM */

	/* ----------------------------------------------------------------
	 *		event loop
	 * ---------------------------------------------------------------- */
	
#endif /* SOUND */

	cdb = db;
	while ((nobj = pad_read(nobj)) > 0) {
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */

		/* dump DB environment */
		/*DumpDrawEnv(&cdb->draw);*/
		/*DumpDispEnv(&cdb->disp);*/
		/*DumpTPage(cdb->draw.tpage);*/
		
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
			
			setXY0(sp, x, y);	/* update vertex */
			AddPrim(ot, sp);	/* apend to OT */
		}
		DrawSync(0);		/* wait for end of drawing */
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */

		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		DrawOTag(cdb->ot);

		/*DumpOTag(cdb->ot);	/* dump OT (for debug) */
#ifdef KANJI
		KanjiFntPrint("‹Ê‚Ì”%d\n", nobj);
		KanjiFntPrint("ŽžŠÔ=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
#ifdef SOUND
		if (cnt > st_load_ave)
		    st_load_ave = cnt;
		FntPrint("total time = %d (%d)\n", cnt, st_load_ave);
#else  /* SOUND */
		FntPrint("total time = %d\n", cnt);
#endif /* SOUND */

#ifdef SOUND
		status = SpuStGetStatus ();
		FntPrint ("SPU streaming  : %s\n",
			  status == SPU_ST_NOT_AVAILABLE ? "NOT AVAILABLE" :
			  status == SPU_ST_IDLE ?          "IDLE" :
			  status == SPU_ST_PREPARE ?       "PREPARE" :
			  status == SPU_ST_TRANSFER ?      "TRANSFER" : "FINAL"
			  );
		FntPrint ("               : %06x\n", SpuStGetVoiceStatus ());
#endif /* SOUND */

		FntFlush(-1);
	}

	/* ----------------------------------------------------------------
	 *		Finalize this sample */
#ifdef SOUND
	SpuSetKey (SPU_OFF, SPU_ALLCH);

	SpuStQuit ();		/* SPU streaming */
	SpuQuit ();
#endif /* SOUND */
	PadStop();

	ResetGraph (3);
	StopCallback ();

	return 0;
}

/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

static void init_prim(DB *db)
{
	u_short	clut[32];		/* CLUT entry */
	SPRT_16	*sp;			/* work */
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
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);			/* set SPRT_16 primitve ID */
		SetSemiTrans(sp, 0);		/* semi-amibient is OFF */
		SetShadeTex(sp, 1);		/* shading&texture is OFF */
		setUV0(sp, 0, 0);		/* texture point is (0,0) */
		sp->clut = clut[i%32];		/* set CLUT */
	}
}	

/*
 * Initialize sprite position and verocity
 */
static init_point(POS *pos)
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();
		pos->y  = rand();
		pos->dx = (rand() % 4) + 1;
		pos->dy = (rand() % 4) + 1;
		pos++;
	}
}

void
spust_prepare (void)
{
    register long i;

    for (i = 0; i < VOICE_LIMIT; i ++) {
	st->voice [i].data_addr = data_start_addr [i];
	st->voice [i].status    = SPU_ST_PLAY;
    }
    played_size = SPU_BUFSIZEHALF;

    st_stop = ST_STOP_INIT_VAL;
    st_stat = 0;
    st_load_ave = 0;
}

long
spust_start (unsigned long voice_bit)
{
    if (st_stat == 0L) {
	spust_prepare ();
    }
    st_stat |= voice_bit;

    return SpuStTransfer (SPU_ST_PREPARE, voice_bit);
}

/*
 * Read controll-pad
 */
static int pad_read(int n)
{
	u_long	padd = PadRead(1);
	
#ifdef SOUND
	static int key_select = False;
	static int key_start  = False;

	static int key_Rup    = False;
	static int key_Rdown  = False;
	static int key_Rright = False;
#ifdef USE_CONFIRM
	static int key_Rleft  = False;
#endif /* USE_CONFIRM */

	static int key_on0    = False;
#if defined(USE_8VOICES) || defined(USE_14VOICES)
	static int key_on1    = False;
	static int key_on2    = False;
	static int key_on3    = False;
#ifdef USE_14VOICES
	static int key_on4    = False;
	static int key_on5    = False;
	static int key_on6    = False;
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */

	static int key_of0    = False;
#if defined(USE_8VOICES) || defined(USE_14VOICES)
	static int key_of1    = False;
	static int key_of2    = False;
	static int key_of3    = False;
#ifdef USE_14VOICES
	static int key_of4    = False;
	static int key_of5    = False;
	static int key_of6    = False;
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */

#define PAD_0 PADLup
#define PAD_1 PADLright
#define PAD_2 PADLdown
#define PAD_3 PADLleft
#define PAD_4 PADRup
#define PAD_5 PADRright
#define PAD_6 PADRdown
#endif /* SOUND */

#ifdef SOUND
	/* <SPU-STREAMING> */
	if (padd & PADstart) {
	    if (key_start == False) {
		key_start = True;
	    }
	    if (padd & PAD_0) {
		if (key_on0 == False) {
		    key_on0 = True;
		    spust_start (SPU_VOICECH (L0) | SPU_VOICECH (R0));
		}
	    } else {
		if (key_on0 == True) {
		    key_on0 = False;
		}
	    }
#if defined(USE_8VOICES) || defined(USE_14VOICES)
	    if (padd & PAD_1) {
		if (key_on1 == False) {
		    key_on1 = True;
		    spust_start (SPU_VOICECH (L1) | SPU_VOICECH (R1));
		}
	    } else {
		if (key_on1 == True) {
		    key_on1 = False;
		}
	    }
	    if (padd & PAD_2) {
		if (key_on2 == False) {
		    key_on2 = True;
		    spust_start (SPU_VOICECH (L2) | SPU_VOICECH (R2));
		}
	    } else {
		if (key_on2 == True) {
		    key_on2 = False;
		}
	    }
	    if (padd & PAD_3) {
		if (key_on3 == False) {
		    key_on3 = True;
		    spust_start (SPU_VOICECH (L3) | SPU_VOICECH (R3));
		}
	    } else {
		if (key_on3 == True) {
		    key_on3 = False;
		}
	    }
#ifdef USE_14VOICES
	    if (padd & PAD_4) {
		if (key_on4 == False) {
		    key_on4 = True;
		    spust_start (SPU_VOICECH (L4) | SPU_VOICECH (R4));
		}
	    } else {
		if (key_on4 == True) {
		    key_on4 = False;
		}
	    }
	    if (padd & PAD_5) {
		if (key_on5 == False) {
		    key_on5 = True;
		    spust_start (SPU_VOICECH (L5) | SPU_VOICECH (R5));
		}
	    } else {
		if (key_on5 == True) {
		    key_on5 = False;
		}
	    }
	    if (padd & PAD_6) {
		if (key_on6 == False) {
		    key_on6 = True;
		    spust_start (SPU_VOICECH (L6) | SPU_VOICECH (R6));
		}
	    } else {
		if (key_on6 == True) {
		    key_on6 = False;
		}
	    }
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */
	} else {
	    if (key_start == True) {
		key_start = False;
	    }
	}

	if (padd & PADselect) {
	    if (key_select == False) {
		key_select = True;
	    }
	    if (padd & PAD_0) {
		if (key_of0 == False) {
		    key_of0 = True;
		    st_stop = (SPU_VOICECH (L0) |
			       SPU_VOICECH (R0));
		}
	    } else {
		if (key_of0 == True) {
		    key_of0 = False;
		}
	    }
#if defined(USE_8VOICES) || defined(USE_14VOICES)
	    if (padd & PAD_1) {
		if (key_of1 == False) {
		    key_of1 = True;
		    st_stop = (SPU_VOICECH (L1) |
			       SPU_VOICECH (R1));
		}
	    } else {
		if (key_of1 == True) {
		    key_of1 = False;
		}
	    }
	    if (padd & PAD_2) {
		if (key_of2 == False) {
		    key_of2 = True;
		    st_stop = (SPU_VOICECH (L2) |
			       SPU_VOICECH (R2));
		}
	    } else {
		if (key_of2 == True) {
		    key_of2 = False;
		}
	    }
	    if (padd & PAD_3) {
		if (key_of3 == False) {
		    key_of3 = True;
		    st_stop = (SPU_VOICECH (L3) |
			       SPU_VOICECH (R3));
		}
	    } else {
		if (key_of3 == True) {
		    key_of3 = False;
		}
	    }
#ifdef USE_14VOICES
	    if (padd & PAD_4) {
		if (key_of4 == False) {
		    key_of4 = True;
		    st_stop = (SPU_VOICECH (L4) |
			       SPU_VOICECH (R4));
		}
	    } else {
		if (key_of4 == True) {
		    key_of4 = False;
		}
	    }
	    if (padd & PAD_5) {
		if (key_of5 == False) {
		    key_of5 = True;
		    st_stop = (SPU_VOICECH (L5) |
			       SPU_VOICECH (R5));
		}
	    } else {
		if (key_of5 == True) {
		    key_of5 = False;
		}
	    }
	    if (padd & PAD_6) {
		if (key_of6 == False) {
		    key_of6 = True;
		    st_stop = (SPU_VOICECH (L6) |
			       SPU_VOICECH (R6));
		}
	    } else {
		if (key_of6 == True) {
		    key_of6 = False;
		}
	    }
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */
	} else {
	    if (key_select == True) {
		key_select = False;
	    }
	}
	/* </SPU-STREAMING> */

	if (key_select == False &&
	    key_start  == False) {

	    if (padd & PADLup)		n += 4;	/* left '+' key up */
	    if (padd & PADLdown)	n -= 4;	/* left '+' key down */
	
	    /* Quit */
	    if (padd & PADRup) {
		if (key_Rup == False) {
		    key_Rup = True;
		}
	    } else {
		if (key_Rup == True) {
		    key_Rup = False;
		    return 0;
		}
	    }
	    
	    /* <SPU-STREAMING> */
#ifdef USE_CONFIRM
	    /* Confirm sound Off */
	    if (padd & PADRleft) {
		if (key_Rleft == False) {
		    key_Rleft = True;
		}
	    } else {
		if (key_Rleft == True) {
		    key_Rleft = False;
		    SpuSetKey (SPU_OFF, SPU_VOICECH (CONFIRM));
		}
	    }
#endif /* USE_CONFIRM */
	    
	    /* SPU streaming prepare/start */
	    if (padd & PADRdown) {
		if (key_Rdown == False) {
		    spust_prepare ();
		    st_stat = all_voices;
		    SpuStTransfer (SPU_ST_PREPARE, all_voices);
		    key_Rdown = True;
		}
	    } else {
		if (key_Rdown == True) {
		    key_Rdown = False;
		}
	    }
	    
	    /* SPU streaming stop (all channel) */
	    if (padd & PADRright) {
		if (key_Rright == False) {
		    key_Rright = True;
		}
	    } else {
		if (key_Rright == True) {
		    st_stop = all_voices;
		    key_Rright = False;
		}
	    }
	    /* </SPU-STREAMING> */
	}
#else
	if(padd & PADLup)	n += 4;		/* left '+' key up */
	if(padd & PADLdown)	n -= 4;		/* left '+' key down */
	
	if(padd & PADL1) 			/* pause */
	    while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return(-1);
#endif /* SOUND */

	limitRange(n, 1, MAXOBJ-1);		/* see libgpu.h */
	return(n);
}		

/*
 * callback
 */
static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));	
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
