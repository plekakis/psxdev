/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
*/
/*  sample for streaming movie from the CD-ROM data.
 *      1) not 16's compliment resolution size
 *         but 24bit mode x's resolution must be even.
 *      2) multi resolution movie data according to it's header
 *  */
/*   Version    Date:
 *  ------------------------------------------------------------------------
 *  1.10        Jul.20 1995 ume
 *                   (changes from tuto0.c ver1.41) 
 *                  Handling of animations having horizontal or vertical axes that are 
 *            not multiples of 16. Improvements to allow playback of multiple sets
 *            of animation data according to their respective parameters.
 **/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>
#include <libpress.h>

#define TRUE    1
#define FALSE   0

/*
 * paramter of MDEC movie */
typedef struct {
    char *fileName;
    int is24bit;
    int startFrame;
    int endFrame;
    int posX;
    int posY;
    int scrWidth;
    int scrHeight;
} MovieInfo;

/*
 * information of movie data
 * NOTICE  on 24 bit mode, positon of X and width of movie must be even. */
static MovieInfo movieInfo[] = {

    /* file name               24bit, start, end,   x,  y, scrW, scrH */

    /* 24 bit */
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 18,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 34,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 50,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  32, 34,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  52, 50,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 128, 34,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 244, 50,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 192, 34,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 372, 50,  640,  240},

    /* 16 bit */
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 18,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 34,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 50,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  32, 34,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  52, 50,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 128, 34,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 244, 50,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 192, 34,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 372, 50,  640,  240},
};
#define NMOVIE  (sizeof(movieInfo) / sizeof(MovieInfo))

/*
 *  16 bit mode / 24 bit mode */
#define VRAMPIX(pixels, is24bit)    ((is24bit)? ((pixels) * 3) / 2: (pixels))   /* the actual pixel number of VRAM */
#define DCT_MODE(is24bit)           ((is24bit)? 3: 2)      /* decode mode */

/* time out parameter for strNextVlc() and strNext() */
#define MOVIE_WAIT 2000

/*
 *  decode environment */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double) */
    int vlcid;          /* current decode buffer id */
    u_short *imgbuf[2]; /* decode image buffer (double) */
    int imgid;          /* corrently use buffer id */
    RECT    rect[2];    /* double buffer orign(upper left point) address
			   on the VRAM (double buffer) */
    int rectid;         /* currently translating buffer id */
    RECT    slice;      /* the region decoded by once DecDCTout() */
    int isdone;         /* the flag of decoding whole frame */
    int is24bit;        /* 24bit mode flag */
} DECENV;
static DECENV   dec;    /* instance of DECENV */

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
#define VLC_BUFF_SIZE (320/2*256)     /* not correct value */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*                              
 *  image buffer(double buffer)
 *  stock the result of MDEC
 *  rectangle of 16(width) by XX(height) */
#define MAX_SCR_HEIGHT 640
#define SLICE_IMG_SIZE (VRAMPIX(16, TRUE) * MAX_SCR_HEIGHT)
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];

/*
 * macro for playing back not 16's compliment size */
#define bound(val, n)               ((((val) - 1) / (n) + 1) * (n))
#define bound16(val)                (bound((val),16))
static int isFirstSlice;    /* flag of the left end slice data */

/*
 *  other valiable */  
static int  Rewind_Switch;  /* the end flag set after last frame */

/*
 *  prototypes */
static int anim(MovieInfo *movie);
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1, MovieInfo *m);
static void strInit(CdlLOC *loc, void (*callback)(), MovieInfo *m);
static void strCallback();
static int strNextVlc(DECENV *dec, MovieInfo *m);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec, MovieInfo *m);
static void strKickCD(CdlLOC *loc);

int main( void )
{
    int i;

    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    while (1) {
        for (i = 0; i < NMOVIE; i++) {
            if(anim(movieInfo + i)==0)
	       return 0;     /* animation subroutine */
        }
    }
}


/*
 *  animation subroutine forground process */
static int anim(MovieInfo *movie)
{
    DISPENV disp;       /* display buffer */
    DRAWENV draw;       /* drawing buffer */
    int id;     /* display buffer id */
    CdlFILE file;
    RECT    clearRect;
    CdlLOC  save_loc;		/* retry location */
    int frame_no;		/* retry frame no */
    
    /* set the left end slice flag */
    isFirstSlice = 1;
    
    /* search file */
    if (CdSearchFile(&file, movie->fileName) == 0) {
        printf("file not found\n");
        PadStop();
	ResetGraph(3);
        StopCallback();
	return 0;
    }
    
    /* set the position of vram */
    strSetDefDecEnv(&dec, VRAMPIX(movie->posX, movie->is24bit), movie->posY,
                VRAMPIX(movie->posX, movie->is24bit), movie->posY+movie->scrHeight, movie);

    /* init streaming system & kick cd */
    strInit(&file.pos, strCallback, movie);
    
    /* expand compressed vlc parameter data */
    DecDCTvlcBuild(vlc_table);
    
    /* VLC decode the first frame */
    while(strNextVlc(&dec, movie) == -1)
      {
	save_loc = file.pos; /* start position */
	strKickCD(&save_loc);
      }
    
    Rewind_Switch = 0;

    SetDispMask(0);		/* mask screen */
    /* clear screen */
    setRECT(&clearRect, 0, 0, VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight*2);
    if (movie->is24bit) {
        ClearImage(&clearRect, 0, 0, 0);        /* clear black on 24 bit mode */
    } else {
        ClearImage(&clearRect, 64, 64, 64); /* clear gray on 16 bit mode */
    }

    while (1) {
        /* start DCT decoding the result of VLC decoded data */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE(movie->is24bit));
        
        /* prepare for recieving the result of DCT decode */
        /* next DecDCTout is called in DecDCToutCallback */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w * bound16(dec.slice.h)/2);
        
        /* decode the next frame's VLC data */
	while(strNextVlc(&dec, movie)== -1)
	  {
	    frame_no = StGetBackloc(&save_loc);	/* get current location */
	    printf("time out in strNext() %d\n",frame_no);
	    if(frame_no>movie->endFrame || frame_no<=0) /* invalid frame no */
	      save_loc = file.pos; /* start position */
	    strKickCD(&save_loc);
	  }
        
        /* wait for whole decode process per 1 frame */
        strSync(&dec, 0);
        
        /* wait for V-Blank */
        VSync(0);
        
        /* swap the display buffer */
        /* notice that the display buffer is the opossite side of
	   decoding buffer */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, dec.rect[id].x - VRAMPIX(movie->posX, movie->is24bit),
            dec.rect[id].y - movie->posY,
            VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight);
        /* SetDefDrawEnv(&draw, dec.rect[id].x, dec.rect[id].y, movie->scrWidth*PPW, movie->scrHeight); */
        
        if (movie->is24bit) {
            disp.isrgb24 = movie->is24bit;
            disp.disp.w = disp.disp.w * 2/3;
        }
        PutDispEnv(&disp);
        /* PutDrawEnv(&draw); */
        SetDispMask(1);     /* display enable */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* stop button pressed exit animation routine */
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
 * init DECENV    buffer0 (x0,y0), buffer1(x1,y1):
 * Initialize decode environment
 *  x0,y0 coordinates for transfer destination of decoded image (buffer 0) 
 *  x1,y1 coordinates for transfer destination of decoded image (buffer 1) 
 **/
static void strSetDefDecEnv(DECENV  *dec, int x0, int y0, int x1, int y1, MovieInfo *movie)
{
  static int isFirst = 1;

  if(isFirst == 1)
    {
      dec->vlcbuf[0] = vlcbuf0;
      dec->vlcbuf[1] = vlcbuf1;
      dec->vlcid     = 0;

      dec->imgbuf[0] = imgbuf0;
      dec->imgbuf[1] = imgbuf1;
      dec->imgid     = 0;
      dec->rectid    = 0;
      dec->isdone = 0;
      isFirst = 0;
    }
  
    /* width and height of rect[] are set dynamicaly according to STR data */
  dec->rect[0].x = x0;
  dec->rect[0].y = y0;
  dec->rect[1].x = x1;
  dec->rect[1].y = y1;
  dec->slice.w = VRAMPIX(16, movie->is24bit);  
  dec->is24bit = movie->is24bit;   
  
  if(dec->rectid == 0)
    {
      dec->slice.x = x0;
      dec->slice.y = y0;
    }
  else
    {
      dec->slice.x = x1;
      dec->slice.y = y1;
    }
}


/*
 * init the streaming environment and start the cdrom */
static void strInit(CdlLOC  *loc, void (*callback)(), MovieInfo *movie)
{
    /* cold reset mdec */
    DecDCTReset(0);

    /* set the callback after 1 block MDEC decoding */
    DecDCToutCallback(callback);
    
    /* set the ring buffer */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* init the streaming library */
    /* end frame is set endless */
    StSetStream(movie->is24bit, movie->startFrame, 0xffffffff, 0, 0);
    
    /* start the cdrom */
    strKickCD(loc);
}


/*
 *  back ground process
 *  callback of DecDCTout() */
static void strCallback()
{
    int mod;
    int id;
    RECT snap_rect;
      
    if (dec.is24bit) {
        extern StCdIntrFlag;
        if(StCdIntrFlag) {
	  StCdInterrupt();  /* on the RGB24 bit mode , call
			       StCdInterrupt manually at this timing */
	  StCdIntrFlag = 0;
        }
    }
/*    
    LoadImage(&dec.slice, (u_long *)dec.imgbuf[dec.imgid]);
*/
    id = dec.imgid;
    snap_rect = dec.slice;
    
    /* switch the id of decoding buffer */
    dec.imgid = dec.imgid? 0:1;

    /* update slice(rectangle) position */
    if (isFirstSlice && (mod = dec.rect[dec.rectid].w % dec.slice.w)) {
        dec.slice.x += mod;
        isFirstSlice = 0;
    } else {
        dec.slice.x += dec.slice.w;
    }
    
    /* remaining slice ? */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* prepare for next slice */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w * bound16(dec.slice.h)/2);
    }
    /* last slice ; end of 1 frame */
    else {
        /* set the decoding done flag */
        dec.isdone = 1;
        isFirstSlice = 1;
        
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
static int strNextVlc(DECENV  *dec, MovieInfo *movie)
{
    int cnt = MOVIE_WAIT;
    u_long  *next;
    static u_long *strNext();
    
    /* get the 1 frame streaming data */
    while ((next = strNext(dec, movie)) == 0) {
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
static u_long *strNext(DECENV  *dec, MovieInfo *movie)
{
    static int  strWidth  = 0;  /* width of the previous frame */
    static int  strHeight = 0;  /* height of the previous frame */
    u_long      *addr;
    StHEADER    *sector;
    int cnt = MOVIE_WAIT;

    /* get the 1 frame streaming data withe TIME-OUT */  
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }

    /* if the frame number greater than the end frame, set the end switch */
    if(sector->frameCount >= movie->endFrame) {
        Rewind_Switch = 1;
    }
    
    /* if the resolution is differ to previous frame, clear frame buffer */
    if(strWidth != sector->width || strHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight*2);
        if (movie->is24bit) {
            ClearImage(&rect, 0, 0, 0);     /* clear black on 24 bit mode */
        } else {
            ClearImage(&rect, 64, 64, 64);  /* clear gray on 16 bit mode */
        }

        strWidth  = sector->width;
        strHeight = sector->height;
    }
    
    /* set DECENV according to the data on the STR format */
    dec->rect[0].w = dec->rect[1].w = VRAMPIX(strWidth, movie->is24bit);
    dec->rect[0].h = dec->rect[1].h = strHeight;
    dec->slice.h   = strHeight;
    
    return(addr);
}


/*
 *  wait for finish decodeing 1 frame with TIME-OUT */
static void strSync(DECENV  *dec, int mode /* VOID */)
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
