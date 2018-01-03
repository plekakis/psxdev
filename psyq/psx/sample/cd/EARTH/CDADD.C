/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright  (C)  1994,5 by Sony Corporation
 *          All rights Reserved
 *
 *  Sample that performs streaming of a movie from a CD-ROM
 *
 *   Version    Date
 *  --------------------------------------------------------------
 *   1.00      Jul,14,1994    yutaka
 *   1.10      Sep,01,1994    suzu
 *   2.00      Feg,07,1996     suzu
 *      2.01            Sep,06,1997     yutaka  (double-speed capabilities) 
 *
 *animInit     initialize background animation processing
 *
 *   Format    int animInit (char *name, int x, int y) 
 * 
 *   Parameters     name animation filename
 *        x, y frame buffer address to which decompressed images are transferred  (upper
left point) 
 *   
 *   Description    Play back the CD-ROM animation image file specified
*         by name and transfer the results to the rectangular
*         region in the frame buffer starting at  (x, y). The size of the
*         animation  (height, width) is dependent on the data to be
*         played back.
 *   
 *   Return value   0 if successful, non-zero if failed
 *
 * animPoll    background animation processing
 *
 *   Format    int animPoll (void) 
 *
 *   Parameters     None
 *   
 *   Description    Polling function for monitoring background animation.
 *             Call once each time the rendering buffer is switched.
 *
 *   Return value   0    waiting for data from CD-ROM (no decoding
 *             of the animation is taking place)  
 *        1    animation being decoded
 *        2    CD-ROM seek taking place (no decoding of animation) 
 *
 *          Movie Sample Program
 *
 * */
 
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>


#define RING_SIZE	32      	/* ring buffer size */
#define VLC_BUFF_SIZE	8196		/* VLC buffer size */
#define SLICE_IMG_SIZE	(16*256)	/* slice image buffer size */

#define START_FRAME	1		/* start frame ID */
#define END_FRAME   	577		/* end frame ID */
#define SCR_WIDTH   	0		/* movie width */
#define SCR_HEIGHT  	0		/* movie height */

#define PPW		1     		/* pixel per short-word */
#define IS_RGB24	0		/* RGB_24 flag is 0 */
#define DCT_MODE	2 		/* 16 bit decode */



/*
 *  decode environment */
typedef struct {
    u_long	*vlcbuf[2];	/* vlc buffer (double) */
    int		vlcid;      	/* vlc buffer ID */
    u_short	*imgbuf[2];	/* decode image buffer (double) */
    int		imgid;    	/* corrently use buffer id */
    RECT	rect[2];	/* double buffer orign (upper left point) */
    int		rectid;		/* currently translating buffer id */
    RECT	slice;		/* the region decoded by once DecDCTout() */
    int		isdone;		/* the flag of decoding whole frame */
} DECENV;

static DECENV   dec;       
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
static int	StrWidth  = 0;  /* resolution of movie */
static int	StrHeight = 0;	/* flags indicating image height change */
static int	Rewind_Switch;  /* flags indicating end of the movie */
static CdlFILE	FileID;		/* CD file handle */

/*
 *  prototypes */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static void strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);


/*
 *  animation subroutine forground process */
int animInit(char *name, int x, int y)
{
    /* search file */
    if (CdSearchFile(&FileID, name) == 0) {
        printf("file not found\n");
        return 1;
    }
    
    /* set the position of vram */
    strSetDefDecEnv(&dec, x, y, x, y);

    /* init streaming system & kick cd */
    strInit(&FileID.pos, strCallback);
    
    /* VLC decode the first frame */
    strNextVlc(&dec);
    
    /* initialize flags */
    Rewind_Switch = 0;
    dec.isdone = 1;
}

int animPoll(void)
{
    static int strNextSync();
    
    /* Finished processing frame? */
    if (dec.isdone == 0)
	    return(0);

    /* Data prepared in ring buffer? */
    if (strNextSync())
	    return(0);
    
    /* Begin decoding VLC */
    dec.isdone = 0;
    
    /* start DCT decoding the result of VLC decoded data */
    DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
    /* prepare for recieving the result of DCT decode */
    /* next DecDCTout is called in DecDCToutCallback */
    DecDCTout(dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
    /* decode the next frame's VLC data */
    strNextVlc(&dec);

    /* check rewind switch */
    if(Rewind_Switch == 1) {
	    Rewind_Switch = 0;
	    strKickCD(&FileID.pos);
	    return(2);
    }
    
    /* return with busy */
    return(1);
}

/*
 * init DECENV    buffer0 (x0,y0), buffer1(x1,y1) :
 * Initialize decode environment
 *  x0,y0 coordinates for transfer destination of decoded image (buffer 0) 
 *  x1,y1 coordinates for transfer destination of decoded image (buffer 1) 
 **/
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
  u_long *ot, *BreakDraw();
  
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
  
  
  /* interrupt current drawing */
  ot = BreakDraw();
  
  /* transfer the decoded data to VRAM */
  LoadImage(&snap_rect, (u_long *)dec.imgbuf[id]);

  /* restart drawing */
  if (ot)
	  DrawOTag(ot);		
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
 *  get the status of 1 frame streaming data
 *  return vale     already setup 1 frame -> 0
 *                  not yet setup 1 frame -> 1 */
static u_long   *addr;
static StHEADER *sector;

static int strNextSync(void)
{
   if (StNextStatus((u_long **)&addr,(u_long **)&sector)==StCOMPLETE)
     return (0);
   else
     return (1);
}

static u_long *strNext(DECENV *dec)
{
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
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT);
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
  if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
