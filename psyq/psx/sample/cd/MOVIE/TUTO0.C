/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  Sample program for playing back MOVIE data from CD-ROM */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>
#include <libpress.h>


#define FILE_NAME   "\\XDATA\\STR\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*
 *  16bit/pixel mode or 24 bit/pixel mode */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* pixel/short word */
#define DCT_MODE    3   /* 24bit decode */
#else
#define PPW 1       /* pixel/short word */
#define DCT_MODE    2   /* 16 bit decode */
#endif

/* time out parameter for strNextVlc() and strNext() */
#define MOVIE_WAIT 2000

/*
 *  decode environment */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double) */
    int vlcid;      /* current decode buffer id */
    u_short *imgbuf[2]; /* decode image buffer (double) */
    int imgid;      /* corrently use buffer id */
    RECT    rect[2];    /* double buffer orign(upper left point) address
			   on the VRAM (double buffer) */
    int rectid;     /* currently translating buffer id */
    RECT    slice;      /* the region decoded by once DecDCTout() */
    int isdone;     /* the flag of decoding whole frame */
} DECENV;
static DECENV   dec;        /* instance of DECENV */

/*
 *  Ring buffer for STREAMING
 *  minmum size is two frame */
#define RING_SIZE   32      /* 32 sectors */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*  VLC table
 *  memory area for vlc parameters
 *  DecDCTvlcBuild() expands compressed data to that area
 *  compressed size is about 3kbyte and decompressed size is about 64kbyte */
DECDCTTAB  vlc_table;
 
/*
 *  VLC buffer(double buffer)
 *  stock the result of VLC decode */
#define VLC_BUFF_SIZE 320/2*256     /* not correct value */
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
static int  StrWidth  = 0;  /* resolution of movie */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* the end flag set after last frame */

/*
 *  prototypes */
static int anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static int  strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

int main( void )
{
    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    while(1) {
        if(anim()==0)
	   return 0;     /* animation subroutine */
    }
}


/*
 *  animation subroutine forground process */
static int anim()
{
    DISPENV disp;       /* display buffer */
    DRAWENV draw;       /* drawing buffer */
    int id;     /* display buffer id */
    CdlFILE file;
    CdlLOC  save_loc;		/* retry location */
    int frame_no;		/* retry frame no */
				                    
    
    /* search file */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
	PadStop();
	ResetGraph(3);
        StopCallback();
        return 0;
    }
    
    /* set the position of vram */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);
    
    /* init streaming system & kick cd */
    strInit(&file.pos, strCallback);

    /* expand compressed vlc parameter data */
    DecDCTvlcBuild(vlc_table);
    
    /* VLC decode the first frame */
    while(strNextVlc(&dec)== -1)
      {
	printf("time out in strNext()");
	save_loc = file.pos; /* start position */
	strKickCD(&save_loc);
      }
    
    Rewind_Switch = 0;
    
    while (1) {
        /* start DCT decoding the result of VLC decoded data */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* prepare for recieving the result of DCT decode */
        /* next DecDCTout is called in DecDCToutCallback */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* decode the next frame's VLC data */
	while(strNextVlc(&dec)== -1)
	  {
	    frame_no = StGetBackloc(&save_loc);	/* get current location */
	    printf("time out in strNext() %d\n",frame_no);
	    if(frame_no>END_FRAME || frame_no<=0) /* invalid frame no */
	      save_loc = file.pos; /* start position */
	    strKickCD(&save_loc);
	  }
        
        /* wait for whole decode process per 1 frame */
        strSync(&dec, 0);
        
        /* wait for V-Vlank */
        VSync(0);
        
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
        
         /* stop button pressed exit animation routine */
       if(PadRead(1) & PADk)
            break;
    }
    
    /* post processing of animation routine */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
    if(Rewind_Switch==0) {
       PadStop();
       ResetGraph(3);
       StopCallback();
       return 0;
       }
    else
       return 1;
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
    /* cold reset mdec */
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
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
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
 *  the decoding data is the next frame's
 *  if time out in strNext() return -1 */
static int strNextVlc(DECENV *dec)
{
    int cnt = MOVIE_WAIT;
    u_long  *next;
    static u_long *strNext();

    /* get the 1 frame streaming data */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return -1;
    }
    
    /* switch the decoding area */
    dec->vlcid = dec->vlcid? 0: 1;
    
    /* VLC decode */
    DecDCTvlc2(next, dec->vlcbuf[dec->vlcid],vlc_table);
    
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
    int     cnt = MOVIE_WAIT;

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
  u_char param;

  param = CdlModeSpeed;
  
 loop:
  /* seek to the destination */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
  while (CdControl(CdlSetmode, &param, 0) == 0);
  VSync(3);  /* wait for 3 VSync when changing the speed */
    /* out the read command with streaming mode */
  if(CdRead2(CdlModeStream2|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
