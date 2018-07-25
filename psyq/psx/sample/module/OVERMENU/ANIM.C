/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  Sample program of movie streaming from CD-ROM
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.00        Jul,14,1994     yutaka
 *  1.10        Sep,01,1994     suzu
 *  1.20        Oct,24,1994     yutaka(anim made as subroutine)
 *  1.30        Jun,02,1995     yutaka(post process is added)
 *  1.40        Jul,10,1995     masa(imgbuf made as double buffer)
 *  1.50        Jul,20,1995     ume(screen clear routine made better)
 *  1.50a	Aug,03,1996     yoshi (for overlay)
 *  1.50b	Mar,04,1996     yoshi (added English comments)
 **/
/*    This was rewritten as a child process. Compile conditionally with OVERLAY.
 *    Called while parent is emitting sound.
 *    Playback is performed at 24bits. */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>


#define FILE_NAME   "\\DATA\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*
 *  specification of the number of colors to decode (16bit/24bit) */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* how many pixels in 1 short word? */
#define DCT_MODE    3   /* decode in 24bit mode */
#else
#define PPW 1       /* how many pixels in 1 short word? */
#define DCT_MODE    2   /* decode in 16 bit mode */
#endif

/*
 *  decode environment variable */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double buffer) */
    int vlcid;      /* Cureent buffer ID for VLC decode operation */
    u_short *imgbuf[2]; /* decode image buffer (double buffer) */
    int imgid;      /* ID of image buffer being used currently */
    RECT    rect[2];    /* coordinates on VRAM (double buffer) */
    int rectid;     /* area retrieved for one DecDCTout */
    RECT    slice;      /* area retrieved for one DecDCTout */
    int isdone;     /* is one frame's worth of data prepared? */
} DECENV;
static DECENV   dec;        /* actual decode environment */

/*
 * ring buffer for streaming
 * stocks data from the CD-ROM
 * reserve capacity for at least 2 frames' worth */
#define RING_SIZE   32      /* unit is sector*/
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*
 *  VLC buffer (double buffer)
 *  stock intermediate data after VLC decoding */
#define VLC_BUFF_SIZE 320/2*256     /* enough space for this sample*/
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*
 * image buffer (double buffer)
 * stock image data from after DCT decoding
 * decode and transfer for rectangles 16 pixels wide */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
/*
 *  other variables */
static int  StrWidth  = 0;  /* size of movie image (horizontal and vertical) */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* end flag: set to 1 when playback has reached a specified frame */

/*
 *  function prototype declaration */
static void anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

#ifdef OVERLAY
child_anim()
#else
main()
#endif
{
    RECT rct;

    /* initialization other than animation are all the same */
#ifdef OVERLAY
/* some tricks are used to allow smooth screen translation */
    VSync(0);
    SetDispMask(0);
    ResetGraph(1);
    setRECT(&rct,0,0,1024,512);
    ClearImage(&rct,0,0,0);
    DrawSync(0);
#else
    ResetCallback();
    CdInit();
    PadInit(0);     /* reset PAD */
    ResetGraph(0);      /* reset GPU */
    SetGraphDebug(0);   /* set debug level */
#endif
    
    anim();     /* animation subroutine */

	return(0);
}


/*
 *  animation subroutine foreground process */
static void anim()
{
    DISPENV disp;       /* display buffer */
    DRAWENV draw;       /* drawing buffer */
    int id;     /* ID of display buffer */
    CdlFILE file;
    
    /* search file */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
        StopCallback();
        PadStop();
        exit();
    }
    
    /* set coordinates on VRAM */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);

    /* initalize and begin streaming */
    strInit(&file.pos, strCallback);
    
    /* peform VLC decoding of the initial frame */
    strNextVlc(&dec);
    
    Rewind_Switch = 0;
    
    while (1) {
        /* DCT encoding is begun on the data for which VLC has been completed 
           (send to MDEC) */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare to receive DCT decode results*/
        /* processing after this is performed with call back routines */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* VLC decoding of the data for the next frame */
        strNextVlc(&dec);
        
        /* wait for decoding of one frame to finish */
        strSync(&dec, 0);
        
        /* wait for V-BLNK */
        VSync(0);
        
        /* swap display buffer  
           note that the display buffer is the reverse of the buffer being decoded */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);
/*      SetDefDrawEnv(&draw, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);*/
        
#if IS_RGB24==1
        disp.isrgb24 = IS_RGB24;
        disp.disp.w = disp.disp.w*2/3;
#endif
        PutDispEnv(&disp);
/*      PutDrawEnv(&draw);*/
        SetDispMask(1);     /* display permission */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* exit animation if the stop button is pressed */
            break;
    }
    
    
    /* post-processing of animation */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
}


/*
 *    initialize decode environment
 *  the transfer coordinates of the image decoded from x0,y0 (buffer 0)
 *  the transfer coordinates of the image decoded from x1,y1 (buffer 1) */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1)
{

    dec->vlcbuf[0] = vlcbuf0;
    dec->vlcbuf[1] = vlcbuf1;
    dec->vlcid     = 0;

    dec->imgbuf[0] = imgbuf0;
    dec->imgbuf[1] = imgbuf1;
    dec->imgid     = 0;

    /* the width/height of rect[] is set from the STR data values */
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
 * initialize streming environment and begin */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* reset MDEC */
    DecDCTReset(0);

    /* define call back for when MDEC has processed one decode block */
    DecDCToutCallback(callback);
    
    /* set ring buffer */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* set up streaming  
       set final frame = infinity */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    
    /* begin streaming read */
    strKickCD(loc);
}


/*
 *    background process
 *  (call back function that is called when DecDCTout() is finished) */
static void strCallback()
{
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* run here if RGB24 */
        StCdIntrFlag = 0;
    }
#endif
    
    /* transfer decode results to the frame buffer */
    LoadImage(&dec.slice, (u_long *)dec.imgbuf[dec.imgid]);
    
    /* switch image decode area */
    dec.imgid = dec.imgid? 0:1;

    /* update slice (rectangular strip) area to next one on the right */
    dec.slice.x += dec.slice.w;
    
    /* any remaining slices? */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
	    /* begin decoding next slice */
        DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
    }
    /* final slice = 1 frame completed */
    else {
        /* indicate completion */
        dec.isdone = 1;
        
        /* update transfer coordinates */
        dec.rectid = dec.rectid? 0: 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y;
    }
}


/*
 *  perform VLC decoding
 *  perform VLC decoding for next frame */
static void strNextVlc(DECENV *dec)
{
    int cnt = WAIT_TIME;
    u_long  *next;
    static u_long *strNext();


    /* retrieve one frame's worth of data */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return;
    }
    
    /* switch VLC decode area */
    dec->vlcid = dec->vlcid? 0: 1;

    /* VLC decode */
    DecDCTvlc(next, dec->vlcbuf[dec->vlcid]);

    /* free up frame area of ring buffer */
    StFreeRing(next);

    return;
}

/*
 *    retrieve data from the ring buffer
 *    (return value)      normal completion = starting address of ring buffer
 *    error occurred = NULL */
static u_long *strNext(DECENV *dec)
{
    u_long      *addr;
    StHEADER    *sector;
    int     cnt = WAIT_TIME;


    /* retrieve data (with timeout) */  
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }

    /* if current frame number is the specified value, then end */   
    if(sector->frameCount >= END_FRAME) {
        Rewind_Switch = 1;
    }
    
    /* if the resolution of the frame is different from that of the previous frame,
          perform ClearImage */
    if(StrWidth != sector->width || StrHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT*2);
        ClearImage(&rect, 0, 0, 0);
        
        StrWidth  = sector->width;
        StrHeight = sector->height;
    }
    
    
    /* change decode environment to match the STR format header */
    dec->rect[0].w = dec->rect[1].w = StrWidth*PPW;
    dec->rect[0].h = dec->rect[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*
 *  wait for the decoding of one frame to finish
 *  monitor time and check for a timeout */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;

    /* wait until the background process sets 'is done' */      
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            /* timeout:  forcibly switch */
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
 *  begin streaming the CDROM from the specified position */
static void strKickCD(CdlLOC *loc)
{
	unsigned char com;

 loop:
  /* Seek the target position */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
    
	com = CdlModeSpeed;
	CdControlB( CdlSetmode, &com, 0 );
	VSync( 3 );

    /* add streaming mode and issue the command */
  if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
