/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *   BG Animation Program (Streaming) */
/*   Ver 1.00  1995 Feb 21
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
*/   
/*  Thease codes are anim subroutines convenient for conbining graphics.
 *
 *  tuto0.c (contains main routine) calls thease subroutines.
 *
 *  Thease subroutine packages have higher priority for streaming than
 *  graphics , so you must allocate double buffers for whole frame image
 *  (16bit/pixcel) and double buffers for VLC decodeing.
 *
 *  If you don't have the priority for the speed of streaming ,
 *  you may reduce memory by preparing for the only single buffer.
 *
 *  The memory size you need for this program is changed
 *  by the size of the image. You must difine the size of the image
 *  by setting WIDTH & HEIGHT in the program. If the value of width
 *  and height written in STR header exceed the value WIDTH & HEIGT
 *  you set , program is halt , so you must set maximum value to
 *  WIDTH and HEIGHT in the program.
 *  
 *  inner function cdrom_play() seeks preset position and plays back
 *  STR format data. But it dosn't have fully error handling. You must
 *  rewrite that routine for the products. */

/*
 * The words number of VLC
 * If you want limit the running time for VLC decoding , you set the
 * maximum words VLC decodes.
 * DecDCTvlc() returns main after decoding VLC_SIZE data. */
#define VLC_SIZE	2048

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libsnd.h>

/*********************** public functions */
void init_anim(void);
void start_anim(CdlFILE *,u_long);
void load_image_for_mdec_data(u_long,u_long);
void poll_anim(CdlFILE *);

/*********************** inner functions */
static void setup_frame();
static void out_callback();
static u_long *get_frame_movie(u_long);
static cdrom_play(CdlLOC *);


#define WIDTH 320		/* the maximum size of image (width) */
#define HIGHT 240		/* the maximum size of image (height) */
#define SLICE  16		/* the width of slize image */



/*
 *	DECODING ENVIRONMENT */
typedef struct {
        u_long	*vlcbuf[2];	/* VLC buffer (double) */
	int	vlcid;		/* current id of VLC buffer */
	u_short	*imgbuf[2];	/* decoded image buffer (double) */
	RECT	rect[2];	/* transfer area (double) */
	int	rectid;		/* current transfer buffer id */
	RECT	slice;		/* slice image of one DecDCTout() */
} DECENV;

static DECENV	dec;		/* instance of DECODE ENVIRONMENT */
static int	MdecFree;	/* MDEC BUSY STATUS */
static int      Frame_ny;   /* STREAMING FLAME DATA NOT READY */
static int      slicew = 0;	/* work of width and height of the
				   streamed iamge */
static int      sliceh = 0;
static int      Vlc_size = 0;	/* the size of one time VLC decoding */

static int      Rewind_Switch;	/* set 1 after playing end frame */
static u_long   EndFrame = 0;	/* the last frame of the streaming */

#define RING_SIZE 24		/* the size of ring buffer (the number of
				   sectors) */

u_long SECT_BUFF[RING_SIZE*SECTOR_SIZE]; /* the instance of ring buffer */


/*
 * back ground process (callback function when DecDCTout() is finished) */
static void out_callback()
{
  MdecFree = 1;			/* when this function is callbacked ,
				   one frame decodeing is done. */
}


/*
 * forground process */	
#define setRECT(r, _x, _y, _w, _h) \
	(r)->x = (_x),(r)->y = (_y),(r)->w = (_w),(r)->h = (_h)


DISPENV	disp;			/* DISPLAY ENVIRONMENT */
u_long	vlcbuf0[WIDTH/2*HIGHT/2];	/* incorrect size */
u_long	vlcbuf1[WIDTH/2*HIGHT/2];	/* incorrect size */
u_short	imgbuf0[WIDTH*HIGHT];	/* 1 frame image buffer */
u_short	imgbuf1[WIDTH*HIGHT];


void init_anim()		/* initialize routines */
{
  RECT rect;
  int i;
  
  DecDCTReset(0);		/* reset MDEC */
  MdecFree = 0;			/* reset the MDEC idle flag */
  Frame_ny = 0;
  Rewind_Switch = 0;		/* initialize stream end flag */
  Vlc_size = 0;
  
  /* set values of decoding structures */
  dec.vlcbuf[0] = vlcbuf0;
  dec.vlcbuf[1] = vlcbuf1;
  dec.vlcid     = 0;
  dec.imgbuf[0]  = imgbuf0;
  dec.imgbuf[1]  = imgbuf1;
  dec.rectid    = 0;
  setRECT(&dec.rect[0], 0,   0, WIDTH, HIGHT);
  setRECT(&dec.rect[1], 0, HIGHT, WIDTH, HIGHT);
  setRECT(&dec.slice,   0,   0,  SLICE, HIGHT);

  setRECT(&rect, 0, 0, WIDTH, HIGHT*2);	/* clear display buff */
  ClearImage(&rect, 0, 0, 0);

  for(i=0;i<WIDTH*HIGHT;i++)	/* clear imgbuf */
    {
      imgbuf0[i]=0;
      imgbuf1[i]=0;
    }
  
  SsInit();		/* SPU lib init */
  SsSetSerialAttr(SS_SERIAL_A,SS_MIX,SS_SON); /* Audio out enable */
  SsSetSerialVol(SS_SERIAL_A,0x7fff,0x7fff);  /* Audio volume set */
  
  /* set the ring buffer */
  StSetRing(SECT_BUFF,RING_SIZE);
  
  /* set the callback of MDEC decodes 1 block */
  DecDCToutCallback(out_callback);
}


/*
 * start streaming by kicking CDROM
 * if the end frame is streamed , end_cdrom is called back.
 *
 * fp         file pointer of the streaming file
 * endFrame   end frame no , notice that you must set 2 or 3 frame
 *            before the real end frame , because if you set the
 *            last frame for end frame and unfortunately skips end frame,
 *            you can't get the end point of the streaming data. */
void start_anim(fp,endFrame)
CdlFILE *fp;
u_long  endFrame;
{
  int ret_val;
  u_long *frame_addr;
  
  StSetStream(0,1,0xffffffff,0,0);
  
  cdrom_play(&fp->pos);

  EndFrame = endFrame;
  
  /* get first frame */
  while((frame_addr = get_frame_movie(EndFrame))==0);
  DecDCTvlcSize(0);  
  /* decode VLC data */
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  
  /* You must free one frame on the ring buffer because you finish decoding */
  StFreeRing(frame_addr);
  
  /* set up input of MDEC one whole frame data */
  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
  
  /* set up output of MDEC one whole frame data */
  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
  
  /* swap ID */
  dec.vlcid = dec.vlcid? 0: 1;
  
  /* get next data , if next data is error , returns 1. */
  /* if the data is error , free that earea and get next frame */
  while((frame_addr=get_frame_movie(EndFrame))==0);
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  StFreeRing(frame_addr);
}

/*
 * get next frame from ring buffer
 * if prepared one frame data , returns the top address of that data
 * if not prepared one frame data , returns NULL */
static u_long *get_frame_movie(endFrame)
u_long endFrame;
{
  u_long   *addr;
  StHEADER *sector;
  
  if(StGetNext(&addr,(u_long **)&sector))
    return(0);			/* not yet prepared one frame data */
  
  if(sector->frameCount > endFrame)
    Rewind_Switch = 1;
  
  /* if the resolution of previous frame is different of current rezolution,
     clear screen */
  if(slicew != sector->width || sliceh != sector->height) {
    RECT    rect;
    setRECT(&rect, 0, 0, WIDTH, HIGHT);
    ClearImage(&rect, 0, 0, 0);
    slicew  = sector->width;
    sliceh  = sector->height;
  }
  
  /* update DECODE ENVIRONMENT according to streaming header */
  dec.rect[0].w = dec.rect[1].w = slicew;
  dec.rect[0].h = dec.rect[1].h = sliceh;
  dec.slice.h   = sliceh;
  
  return(addr);
}


/*
 * transfer whole screen slice image to VRAM at once
 * arguments are the location of the VRAM */
void load_image_for_mdec_data(xo,yo)
u_long xo,yo;
{
  int i,x;
  RECT tmprect;
  
  for(i=0;i<slicew/16;i++)
    {
      x = i*16;
      tmprect.x = x+xo;
      tmprect.y = yo;
      tmprect.w = 16;
      tmprect.h = sliceh;
      
      LoadImage(&tmprect, (u_long*)(dec.imgbuf[dec.rectid? 0: 1]+x*sliceh));
      DrawSync(0);		/* wait for complete transfering */
    }
}


/* 
 *  start decoding next frame
 *  MdecFree and Frame_ny are status flags
 *  MdecFree is set when MDEC is idle
 *  Frame_ny is set when not preparing one frame data in the ring buffer */
static void setup_frame()
{
  static u_long *frame_addr;
  
  if(Vlc_size == 0)
    {
      if(Frame_ny == 0)
	{
	  dec.rectid = dec.rectid? 0: 1;
	  
	  /* tarnsfer VLC decoded data */
	  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
	  
	  /* prepare for recieving first decoded block */
	  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
	  
	  /* swap id */
	  dec.vlcid = dec.vlcid? 0: 1;
	  MdecFree = 0;
	}
      
      if((frame_addr=get_frame_movie(EndFrame))==0)
	{
	  Frame_ny = 1;
	  return;
	}
      Frame_ny = 0;
      
      DecDCTvlcSize(VLC_SIZE);
      Vlc_size = DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
    }
  else
    Vlc_size = DecDCTvlc(0,0);
  
  if(Vlc_size==0)
    if(StFreeRing(frame_addr))
      printf("FREE ERROR\n");
}

/*
 * execute animation routine by polling
 * if MDEC is not idle and not finish next frame's VLC decoding
 * (MdecFree == 0 & Frame_ny == 0) , return imediately */
void poll_anim(fp)
CdlFILE *fp;
{
  if(Rewind_Switch) /* if after end frame , jumpt to start location */
    {
      cdrom_play(&fp->pos);
      Rewind_Switch = 0;
    }
  
  /* if you execute streaming routine , you call setup_frame() */
  if(MdecFree || Frame_ny || Vlc_size)
    setup_frame();
}


/*
 * start animation by seeking loc
 * Notice that this routine have no error handling , so you must coding
 * error handling routine for the product */
static cdrom_play(CdlLOC *loc)
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
