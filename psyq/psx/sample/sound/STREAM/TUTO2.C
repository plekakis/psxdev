/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  Sample program for playing back movie data from CD-ROM */

/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto2.c,v $
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

/*
 * Sound and SPU streaming
 */
#define SOUND			/* sound available */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>


#define FILE_NAME   "\\XDATA\\STR\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*
 *  16bit/pixel mode or 24 bit/pixel mode */
#define IS_RGB24    0		/* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2			/* pixel/short word */
#define DCT_MODE    3		/* 24bit decode */
#else
#define PPW 1			/* pixel/short word */
#define DCT_MODE    2		/* 16 bit decode */
#endif

/*
 *  decode environment */
typedef struct {
    u_long  *vlcbuf[2];		/* VLC buffer (double) */
    int vlcid;			/* current decode buffer id */
    u_short *imgbuf[2];		/* decode image buffer (double) */
    int imgid;			/* corrently use buffer id */
    RECT    rect[2];		/* double buffer orign(upper left point) address
				   on the VRAM (double buffer) */
    int rectid;			/* currently translating buffer id */
    RECT    slice;		/* the region decoded by once DecDCTout() */
    int isdone;			/* the flag of decoding whole frame */
} DECENV;
static DECENV   dec;		/* instance of DECENV */

/*
 *  Ring buffer for STREAMING
 *  minmum size is two frame */
#define RING_SIZE   32      /* 32 sectors */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*
 *  VLC buffer(double buffer)
 *  stock the result of VLC decode */
#define VLC_BUFF_SIZE 320/2*256	/* not correct value */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*
 *  image buffer(double buffer)
 *  stock the result of MDEC
 *  rectangle of 16(width) by XX(height) */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
/*
 *  Other variables */
static int  StrWidth  = 0;	/* resolution of movie */
static int  StrHeight = 0;  
static int  Rewind_Switch;	/* the end flag set after last frame */

/*
 *  prototypes */
static int anim();

#define ANIM_CONTINUE 0
#define ANIM_REWIND   1
#define ANIM_QUIT     2

static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

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
#if defined(USE_14VOICES)
    TOP_ADDR + DATA_SIZE * 8,	/* 4L */
    TOP_ADDR + DATA_SIZE * 9,	/* 4R */
    TOP_ADDR + DATA_SIZE * 10,	/* 5L */
    TOP_ADDR + DATA_SIZE * 11,	/* 5R */
    TOP_ADDR + DATA_SIZE * 12,	/* 6L */
    TOP_ADDR + DATA_SIZE * 13	/* 6R */
#endif /* USE_14VOICES */
#endif /* USE_8VOICES || USE_14VOICES */
};

#define _L0      0
#define _R0      1
#define _L1      2
#define _R1      3
#define _L2      4
#define _R2      5
#define _L3      6
#define _R3      7
#define _L4      8
#define _R4      9
#define _L5     10
#define _R5     11
#define _L6     12
#define _R6     13
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

unsigned long all_voices = (SPU_VOICECH (_L0) | SPU_VOICECH (_R0)
#if defined(USE_8VOICES) || defined(USE_14VOICES)
			    | SPU_VOICECH (_L1) | SPU_VOICECH (_R1)
			    | SPU_VOICECH (_L2) | SPU_VOICECH (_R2)
			    | SPU_VOICECH (_L3) | SPU_VOICECH (_R3)
#ifdef USE_14VOICES
			    | SPU_VOICECH (_L4) | SPU_VOICECH (_R4)
			    | SPU_VOICECH (_L5) | SPU_VOICECH (_R5)
			    | SPU_VOICECH (_L6) | SPU_VOICECH (_R6)
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

static int pad_read(void);

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
#ifdef SOUND
    SpuVoiceAttr s_attr;
    SpuCommonAttr c_attr;
    long i;
    /* <SPU-STREAMING> */
    unsigned long buffer_addr [SPU_MALLOC_MAX];
    /* </SPU-STREAMING> */
#endif /* SOUND */
    
    /* ----------------------------------------------------------------
     *		Initialize interrupt environment. */

    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
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
    
    /* Initialize SPU streaming */
    
    st = SpuStInit (0);
    
     /* Set some callback functions */
    
    /* For finishing SPU streaming preparation */
    (void) SpuStSetPreparationFinishedCallback ((SpuStCallbackProc)
						spustCB_preparation_finished);
    /* For next transferring */
    (void) SpuStSetTransferFinishedCallback ((SpuStCallbackProc)
					     spustCB_transfer_finished);
    /* For finising SPU streaming with some voices */
    (void) SpuStSetStreamFinishedCallback ((SpuStCallbackProc)
					   spustCB_stream_finished);
    
     /* Allocate buffers in sound buffer */
    
    /* for SPU streaming itself */
    for (i = 0; i < VOICE_LIMIT; i ++) {
	if ((buffer_addr [i] = SpuMalloc (SPU_BUFSIZE)) == -1) {
	    printf ("SpuMalloc : %d\n", i);
	    return 0;	/* ERROR */
	}
    }
    
#ifdef USE_CONFIRM
    /* for the sound when finish SPU streaming */
    if ((buffer_addr [CONFIRM] = SpuMalloc (SIN_DATA_SIZE)) == -1) {
	printf ("SpuMalloc : %d\n", CONFIRM);
	return 0; /* ERROR */
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
    
    for (i = _L0; i < VOICE_LIMIT; i += 2) {
	s_attr.voice = SPU_VOICECH (i);
	s_attr.addr  = buffer_addr [i];
	SpuSetVoiceAttr (&s_attr);
    }
    
    /* attribute values: R-ch */
    s_attr.volume.left  = 0x0;
    s_attr.volume.right = 0x3fff;
    for (i = _R0; i < VOICE_LIMIT; i += 2) {
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
    
    /* animation subroutine */
    while(anim () == ANIM_CONTINUE)
	;			

#ifdef SOUND
    /* wait for finishing all streams */
    while (SpuStGetStatus () != SPU_ST_IDLE)
	;

    /* ----------------------------------------------------------------
     *		Finalize this sample */

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
 *  animation subroutine forground process */
static int anim()
{
    DISPENV disp;		/* display buffer */
    DRAWENV draw;		/* drawing buffer */
    int id;			/* display buffer id */
    CdlFILE file;
#ifdef SOUND
    long status;
#endif /* SOUND */
    long pad_status = ANIM_CONTINUE;
    
    /* search file */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
#ifdef SOUND
	SpuStQuit ();		/* SPU streaming */
	SpuQuit ();
#endif /* SOUND */
        PadStop();
        StopCallback();
	return ANIM_QUIT;
    }
    
    /* set the position of vram */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);

    /* init streaming system & kick cd */
    strInit(&file.pos, strCallback);
    
    /* VLC decode the first frame */
    strNextVlc(&dec);
    
    Rewind_Switch = 0;
    
    while (1) {
        /* start DCT decoding the result of VLC decoded data */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare for recieving the result of DCT decode */
        /* next DecDCTout is called in DecDCToutCallback */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* decode the next frame's VLC data */
        strNextVlc(&dec);
        
        /* wait for whole decode process per 1 frame */
        strSync(&dec, 0);
        
        /* wait for V-Blank */
        VSync(0); /**/

        /* swap the display buffer */
        /* notice that the display buffer is the opossite side of
	   decoding buffer */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);
/*      SetDefDrawEnv(&draw, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);*/
        
#if IS_RGB24==1
        disp.isrgb24 = IS_RGB24;
        disp.disp.w = disp.disp.w*2/3;
#endif
        PutDispEnv(&disp);
/*      PutDrawEnv(&draw);*/
        SetDispMask(1);     /* display enable */
        
        if(Rewind_Switch == 1)
            break;

#ifdef SOUND
        if((pad_status = pad_read ()) != ANIM_CONTINUE)
            break;
#else  /* SOUND */
	/* if SELECT button is pressed, exit animation routine */
        if(PadRead(1) & PADselect)
            break;
#endif /* SOUND */
    }
    
    /* post processing of animation routine */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);

    return (pad_status == ANIM_QUIT) ? ANIM_QUIT : ANIM_CONTINUE;
}


/*
 * init DECENV    buffer0(x0,y0),buffer1(x1,y1) */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1)
{

    dec->vlcbuf[0] = vlcbuf0;
    dec->vlcbuf[1] = vlcbuf1;
    dec->vlcid     = 0;

    dec->imgbuf[0] = imgbuf0;
    dec->imgbuf[1] = imgbuf1;
    dec->imgid     = 0;

    /* width and height of rect[] are set dynamicaly according to STR data */
    dec->rect[0].x = x0;
    dec->rect[0].y = y0;
    dec->rect[1].x = x1;
    dec->rect[1].y = y1;
    dec->rectid    = 0;

    dec->slice.x = x0;
    dec->slice.y = y0;
    dec->slice.w = 16*PPW;

    dec->isdone    = 0;
}


/*
 * init the streaming environment and start the cdrom */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* cold reset MDEC */
    DecDCTReset(0);

    /* set the callback after 1 block MDEC decoding */
    DecDCToutCallback(callback);
    
    /* set the ring buffer */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* init the streaming library */
    /* end frame is set endless */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    /* start the cdrom */
    strKickCD(loc);
}


/*
 *  back ground process
 *  callback of DecDCTout() */
static void strCallback()
{
    RECT snap_rect;
    int  id;
  
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* on the RGB24 bit mode , call
			       StCdInterrupt manually at this timing */
        StCdIntrFlag = 0;
    }
#endif
    id = dec.imgid;
    snap_rect = dec.slice;
  
    /* switch the id of decoding buffer */
    dec.imgid = dec.imgid? 0:1;

    /* update slice(rectangle) position */
    dec.slice.x += dec.slice.w;
    
    /* remaining slice ? */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* prepare for next slice */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
    }
    /* last slice ; end of 1 frame */
    else {
        /* set the decoding done flag */
        dec.isdone = 1;
        
        /* update the position on VRAM */
        dec.rectid = dec.rectid? 0: 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y;
    }
  
    /* transfer the decoded data to VRAM */
    LoadImage(&snap_rect, (u_long *)dec.imgbuf[id]);
}


/*
 *  execute VLC decoding
 *  the decoding data is the next frame's */
static void strNextVlc(DECENV *dec)
{
    int cnt = WAIT_TIME;
    u_long  *next;
    static u_long *strNext();

    /* get the 1 frame streaming data */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return;
    }
    
    /* switch the decoding area */
    dec->vlcid = dec->vlcid? 0: 1;

    /* VLC decode */
    DecDCTvlc(next, dec->vlcbuf[dec->vlcid]);

    /* free the ring buffer */
    StFreeRing(next);

    return;
}

/*
 *  get the 1 frame streaming data
 *  return vale     normal end -> top address of 1 frame streaming data
 *                  error      -> NULL */
static u_long *strNext(DECENV *dec)
{
    u_long      *addr;
    StHEADER    *sector;
    int     cnt = WAIT_TIME;

    /* get the 1 frame streaming data withe TIME-OUT */
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }

    /* if the frame number greater than the end frame, set the end switch */
    if(sector->frameCount >= END_FRAME) {
        Rewind_Switch = 1;
    }
    
    /* if the resolution is differ to previous frame, clear frame buffer */
    if(StrWidth != sector->width || StrHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT*2);
        ClearImage(&rect, 0, 0, 0);
        
        StrWidth  = sector->width;
        StrHeight = sector->height;
    }
    
    /* set DECENV according to the data on the STR format */
    dec->rect[0].w = dec->rect[1].w = StrWidth*PPW;
    dec->rect[0].h = dec->rect[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*
 *  wait for finish decodeing 1 frame with TIME-OUT */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;

    /* wait for the decod is done flag set by background process */
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            /* if timeout force to switch buffer */
            printf("time out in decoding !\n");
            dec->isdone = 1;
            dec->rectid = dec->rectid? 0: 1;
            dec->slice.x = dec->rect[dec->rectid].x;
            dec->slice.y = dec->rect[dec->rectid].y;
        }
    }
    dec->isdone = 0;
}


/*
 *  start streaming */
static void strKickCD(CdlLOC *loc)
{
loop:
    /* seek to the destination */
    while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
    
    /* out the read command with streaming mode */
    if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
	goto loop;
}

#ifdef SOUND
/* <SPU-STREAMING> */
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
/* </SPU-STREAMING> */

/*
 * Read controll-pad
 */
static
int
pad_read(void)
{
    u_long	padd = PadRead(1);
    
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
#if defined(USE_14VOICES)
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
#if defined(USE_14VOICES)
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

    /* <SPU-STREAMING> */
    if (padd & PADstart) {
	if (key_start == False) {
	    key_start = True;
	}
	if (padd & PAD_0) {
	    if (key_on0 == False) {
		key_on0 = True;
		spust_start (SPU_VOICECH (_L0) | SPU_VOICECH (_R0));
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
		spust_start (SPU_VOICECH (_L1) | SPU_VOICECH (_R1));
	    }
	} else {
	    if (key_on1 == True) {
		key_on1 = False;
	    }
	}
	if (padd & PAD_2) {
	    if (key_on2 == False) {
		key_on2 = True;
		spust_start (SPU_VOICECH (_L2) | SPU_VOICECH (_R2));
	    }
	} else {
	    if (key_on2 == True) {
		key_on2 = False;
	    }
	}
	if (padd & PAD_3) {
	    if (key_on3 == False) {
		key_on3 = True;
		spust_start (SPU_VOICECH (_L3) | SPU_VOICECH (_R3));
	    }
	} else {
	    if (key_on3 == True) {
		key_on3 = False;
	    }
	}
#if defined(USE_14VOICES)
	if (padd & PAD_4) {
	    if (key_on4 == False) {
		key_on4 = True;
		spust_start (SPU_VOICECH (_L4) | SPU_VOICECH (_R4));
	    }
	} else {
	    if (key_on4 == True) {
		key_on4 = False;
	    }
	}
	if (padd & PAD_5) {
	    if (key_on5 == False) {
		key_on5 = True;
		spust_start (SPU_VOICECH (_L5) | SPU_VOICECH (_R5));
	    }
	} else {
	    if (key_on5 == True) {
		key_on5 = False;
	    }
	}
	if (padd & PAD_6) {
	    if (key_on6 == False) {
		key_on6 = True;
		spust_start (SPU_VOICECH (_L6) | SPU_VOICECH (_R6));
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
		st_stop = (SPU_VOICECH (_L0) |
			   SPU_VOICECH (_R0));
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
		st_stop = (SPU_VOICECH (_L1) |
			   SPU_VOICECH (_R1));
	    }
	} else {
	    if (key_of1 == True) {
		key_of1 = False;
	    }
	}
	if (padd & PAD_2) {
	    if (key_of2 == False) {
		key_of2 = True;
		st_stop = (SPU_VOICECH (_L2) |
			   SPU_VOICECH (_R2));
	    }
	} else {
	    if (key_of2 == True) {
		key_of2 = False;
	    }
	}
	if (padd & PAD_3) {
	    if (key_of3 == False) {
		key_of3 = True;
		st_stop = (SPU_VOICECH (_L3) |
			   SPU_VOICECH (_R3));
	    }
	} else {
	    if (key_of3 == True) {
		key_of3 = False;
	    }
	}
#if defined(USE_14VOICES)
	if (padd & PAD_4) {
	    if (key_of4 == False) {
		key_of4 = True;
		st_stop = (SPU_VOICECH (_L4) |
			   SPU_VOICECH (_R4));
	    }
	} else {
	    if (key_of4 == True) {
		key_of4 = False;
	    }
	}
	if (padd & PAD_5) {
	    if (key_of5 == False) {
		key_of5 = True;
		st_stop = (SPU_VOICECH (_L5) |
			   SPU_VOICECH (_R5));
	    }
	} else {
	    if (key_of5 == True) {
		key_of5 = False;
	    }
	}
	if (padd & PAD_6) {
	    if (key_of6 == False) {
		key_of6 = True;
		st_stop = (SPU_VOICECH (_L6) |
			   SPU_VOICECH (_R6));
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
	
	/* Rewind */
	if (padd & PADRup) {
	    if (key_Rup == False) {
		key_Rup = True;
	    }
	} else {
	    if (key_Rup == True) {
		key_Rup = False;
		return ANIM_REWIND;
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

    if ((padd & PADstart) &&
	(padd & PADselect)) {
	st_stop = all_voices;
	return ANIM_QUIT;
    } else {
	return ANIM_CONTINUE;
    }
}		
#endif /* SOUND */
