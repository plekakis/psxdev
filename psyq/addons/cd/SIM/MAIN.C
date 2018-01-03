/*
 * $PSLibId: Runtime Library Versin 3.0$
 */
/*
 *			ON Memory STR Data Viewer
 *	
 *	Preview STR video data developed in ON memory. STR data 
 *   created by the movie converter can be viewed as is.
 *	
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		Jul,08,1994	suzu
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define RGB24
#ifdef RGB24
#define PPW	3/2	/* How many pixels per short word? (Pixels Per Word)*/
#define IPPW	2/3	/* 1/PPW */
#define MODE	1	/* Decode in 24-bit mode*/
#else
#define PPW	1	/* How many pixels per short word?*/
#define IPPW	1	/* 1/PPW */
#define MODE	0	/* Decode in 16-bit mode*/
#endif

/*
 *	Decoding environment
 */
typedef struct {
	u_long	*vlcbuf[2];	/* VLC  buffer (double buffer)*/
	int	vlcid;		/* Buffer ID during decoding of current VLC*/
	u_short	*imgbuf;	/* Decode image buffer (single)*/	
	RECT	rect[2];	/* Transfer area (double buffer) */
	int	rectid;		/* Buffer ID during current transfer */
	RECT	slice;		/* Area fetched by one DecDCTout*/
	int	d_wid;		/* Width of display area */
} DECENV;

static DECENV		dec;		/* Decoding environment entity*/
static volatile int	isEndOfFlame;	/* Goes to 1 at the end of the frame. */

/*
 * Foreground process
 */	
u_long	vlcbuf0[256*256];	/* Appropriate size*/
u_long	vlcbuf1[256*256];	/* Appropriate size*/
u_short	imgbuf[16*PPW*240*2];	/* 1 short fence*/

static int movie(void);
static int strSync(int mode);
static void strCallback(void);
static void strRewind(DECENV *dec);
static u_long *strNext(void);

main()
{
	PadInit(0);
	ResetGraph(0);		/* Reset GPU*/
	SetGraphDebug(0);	/* Set debug level*/
	SetDispMask(1);		/* Display enabled*/
	
	/* Clear frame buffer. */
	{
		RECT	clear;
		setRECT(&clear, 0, 0, 640, 480);
		ClearImage(&clear, 0, 0, 0);
	}
	
	/* Load font. */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 16, 1, 512));

	/* Set value in decoding structure. */
	dec.vlcbuf[0] = vlcbuf0;
	dec.vlcbuf[1] = vlcbuf1;
	dec.vlcid     = 0;
	dec.imgbuf    = imgbuf;
	dec.rectid    = 0;

	/* Start streaming. */
	while (movie() == 0);

	/* Exit*/
	PadStop();
	exit();
}

static int movie(void)
{
	DISPENV	disp;
	DRAWENV	draw;
	
	int	id, padd;
	u_long	*next;
	
	DecDCTReset(0);			/* Reset MDEC*/
	isEndOfFlame = 0;		/* Lower flag. */
	strRewind(&dec);		/* Frame rewind. */
	DecDCToutCallback(strCallback);/* Define callback. */
	
	/* First, release initial VLC. */
	DecDCTvlc(strNext(), dec.vlcbuf[dec.vlcid]);
	
	while (1) {

		/* Send VLC's completed data*/
		DecDCTin(dec.vlcbuf[dec.vlcid], MODE);
		
		/* Prepare for receiving initial short volume. */
		/* After second time, execute inside the callback().*/
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);

		/* Swap IDs. */
		dec.vlcid = dec.vlcid? 0: 1;

		/* Release next VLC. */
		if ((next = strNext()) == 0)
			return(0);

		DecDCTvlc(next, dec.vlcbuf[dec.vlcid]);

		/*		---			*/
		/* Application code is located here. */
		/*		---			*/
		if (PadRead(1) & PADk)
			return(-1);
		
		/* Wait for completion of data decoding*/
		if (strSync(0) != 0)
			return(-1);
		
		/* Wait for V-BLNK*/
		VSync(0);
		
		/* Swap display buffers*/
		/* Take note that the display buffer is on the opposite
	side from the transfer buffer. */
		id = dec.rectid? 0: 1;
		
		/* Swap drawing environment */
		SetDefDispEnv(&disp, 0, id==0? 0:240, dec.d_wid, 240);
		SetDefDrawEnv(&draw, 0, id==0? 0:240, dec.d_wid, 240);
#ifdef RGB24
		disp.isrgb24 = 1;
#endif		
		PutDispEnv(&disp);
		PutDrawEnv(&draw);
		FntFlush(-1);
	}
}

static int strSync(int mode)
{
	int	cnt = WAIT_TIME;
	while (isEndOfFlame == 0) 
		 if (cnt-- == 0)
			 return(-1);
	isEndOfFlame = 0;
	return(0);
}

/* * Background process
 * (Callback function to be called when DecDCTout() is completed)*/
static void strCallback(void)
{
	/* Send decoding result to frame buffer*/
	LoadImage(&dec.slice, (u_long *)dec.imgbuf);
	
	/* Update short fence rectangle area to next right position*/
	dec.slice.x += 16*PPW;

	/* If that is still not enough, */
	if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
		/* receive next short fence.*/
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	}
	/* When one frame is completed*/
	else {
		/* send notification of completion.*/
		isEndOfFlame = 1;
		
		/* Update ID*/
		dec.rectid  = dec.rectid? 0: 1;
		dec.slice.x = dec.rect[dec.rectid].x;
		dec.slice.y = dec.rect[dec.rectid].y;
	}
}		

/*
 *	Read in next streaming data (really comes from CD-ROM)
 */
#define STRADDR		(CDSECTOR *)0xa0200000	/* STR data address */
#define USRSIZE		(512-8)		/* Data size per sector */

typedef struct {
	u_short	id;			/* always 0x0x0160 */
	u_short	type;			
	u_short	secCount;	
	u_short	nSectors;
	u_long	frameCount;
	u_long	frameSize;
	
	u_short	width;
	u_short	height;
	u_char	reserved[12];
	
	u_long	data[USRSIZE];		/* User data */
} CDSECTOR;				/* CD-ROM STR structure*/

CDSECTOR	*Sector;		/* Current sector position */

static void strRewind(DECENV *dec)
{
	/* Rewind pointer */
	Sector = STRADDR;
		
	/* Change decoding environment to match mini header*/
	/* Drawing environment*/
	if (Sector->width <= 256)	dec->d_wid = 256;
	else if (Sector->width <= 320)	dec->d_wid = 320;
	else if (Sector->width <= 512)	dec->d_wid = 512;
	else 				dec->d_wid = 640;
		
	/* Transfer area */
	dec->rect[0].x = dec->rect[1].x = (dec->d_wid-Sector->width)/2;
	dec->rect[0].y = (240-Sector->height)/2;
	dec->rect[1].y = (240-Sector->height)/2 + 240;
	
	dec->rect[0].w = dec->rect[1].w = Sector->width*PPW;
	dec->rect[0].h = dec->rect[1].h = Sector->height;

	setRECT(&dec->slice,
		dec->rect[0].x, dec->rect[0].y, 16*PPW, Sector->height);
}

static u_long *strNext(void)
{
	static u_long data[20*USRSIZE];	/* Data with mini-header removed */
	
	int	i, j, len;
	u_long	*sp, *dp;
	
#ifndef RGB24		
	FntPrint("%d: %d sect,(%d,%d)\n",
	       Sector->frameCount, Sector->nSectors,
	       Sector->width, Sector->height);
#endif	
	/* Create user data with mini-header removed. */
	dp  = data;
	len = Sector->nSectors;
	while (len--) {
		/* Check meaningful headers.*/
		if (Sector->id != 0x0160) 
			return(0);
		
		/* Copy data. */
		for (sp = Sector->data, i = 0; i < USRSIZE; i++) 
			*dp++ = *sp++;
		Sector++;
	}
	return((u_long *)data);
}
