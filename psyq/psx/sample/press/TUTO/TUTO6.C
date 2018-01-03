/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto6 simplest sample
 */
/*	Complete background on-memory movie decompression (with frame
 *	double buffering). Notice VLC decoding is done in CPU (foreground)
 *	 */	
/*		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

/*
 * Decode Environment. notice that image buffer in main memory is single */
typedef struct {
	/* VLC buffer (double)*/
	u_long	*vlcbuf[2];	
	
	/* current VLC buffer ID*/
	int	vlcid;		
	
	/* decoded image buffer (single)*/
	u_long	*imgbuf;	
	
	/* frame buffer area (double)*/
	RECT	rect[2];	
	
	/* current decompressing buffer ID*/
	int	rectid;		
	
	/* one slice area of one DecDCTout*/
	RECT	slice;		
	
} DECENV;
static DECENV		dec;		
	
/* flag indicating the enf of decoding. 'volatile' option is necessary. */
static volatile int	isEndOfFlame;	

/*
 * background operation. This function is called when DecDCTout is finished */
static void out_callback(void)
{
	/* transfer decoded data to frame buffer */
	LoadImage(&dec.slice, dec.imgbuf);
	
	/* update sliced image area*/
	dec.slice.x += 16;

	/* if decoding is not complete*/
	if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
		/* get next slice image from MDEC*/
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	}
	/* when all frame image is decompressed*/
	else {
		/* notify the end of decompression*/
		isEndOfFlame = 1;
		
		/* update ID of each buffer*/
		dec.rectid = dec.rectid? 0: 1;
		dec.slice.x = dec.rect[dec.rectid].x;
		dec.slice.y = dec.rect[dec.rectid].y;
	}
}		

/*
 * foreground operation*/	
/* following work buffer size should be dynamically alocated using malloc().
 * the size of buffer can be known by DecDCTvlcBufSize() */	
static u_long	vlcbuf0[256*256];	
static u_long	vlcbuf1[256*256];	
static u_long	imgbuf[16*240/2];	/* 1 slice size*/

static void StrRewind(void);
static u_long *StrNext(void);

/*
 * VLC maximum count of decoding 
 * DecDCTvlc() is foreground and if it occupies CPU for a long time,
 * set maximum VLC decode size here. DecDCTvlc returns after
 * decompression VLC_SIZE words. You can restart VLC decoding again
 * by calling DecDCTvlc() again
 * */
#define VLC_SIZE	1024	

void tuto6(void)
{
	DISPENV	disp;			/* display environment*/
	DRAWENV	draw;			/* drawing environment*/
	RECT	rect;			/* LoadImage Rectangle */
	void	out_callback();		/* callback*/
	int	id;			/* buffer ID */
	u_long	padd;			/* controller response */
	u_long	*next, *StrNext();	/* CD-ROM simulator*/
	int	isvlcLeft;		/* VLC flag */	
	int	isend = 0;		/* end of program */
	DECDCTTAB	table;		/* VLC table */
	
	DecDCTvlcBuild(table);	/* build table*/
	isEndOfFlame = 0;	/* clear flag*/

	/* clear frame buffer*/
	setRECT(&rect, 0, 0, 640,480);
	ClearImage(&rect, 0, 0, 0);

	/* initialize font environment*/
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 16, 1, 512));
	
	/* intialize decoding environment*/
	dec.vlcbuf[0] = vlcbuf0;
	dec.vlcbuf[1] = vlcbuf1;
	dec.vlcid     = 0;
	dec.imgbuf    = imgbuf;
	dec.rectid    = 0;
	
	setRECT(&dec.rect[0], 0,  32, 256, 176);
	setRECT(&dec.rect[1], 0, 272, 256, 176);
	setRECT(&dec.slice,   0,  32,  16, 176);
		
	/* define callback*/
	DecDCToutCallback(out_callback);
	
	/* rewind CD-ROM simulator*/
	StrRewind();
	
	/* decode the first VLC*/
	DecDCTvlcSize2(0);
	DecDCTvlc2(StrNext(), dec.vlcbuf[dec.vlcid], table);

	/* main loop*/
	while (isend == 0) {
		/* send run-level (result of VLC decoding) to MDEC */
		DecDCTin(dec.vlcbuf[dec.vlcid], 0);	
	
		/* swap VLC ID*/
		dec.vlcid = dec.vlcid? 0: 1;		

		/* start recieving the first decoded slice image.
		 * next one is called in the callback. */
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	
		/* read next BS from CD-ROM simulator*/
		if ((next = StrNext()) == 0)	
			break;
		
		/* decode first VLC_SIZE words of VLC buffer */ 
		DecDCTvlcSize2(VLC_SIZE);
		isvlcLeft = DecDCTvlc2(next, dec.vlcbuf[dec.vlcid], table);

		/* display consumed time*/
		FntPrint("slice=%d,", VSync(1));
		
		/* wait for end of decoding*/
		do {
			/* decode next VLC_SIZE words of VLC buffer */ 
			if (isvlcLeft) {
				isvlcLeft = DecDCTvlc2(0, 0, table);
				FntPrint("%d,", VSync(1));
			}

			/* watch controler*/
			if ((padd = PadRead(1)) & PADselect) 
				isend = 1;
			
		} while (isvlcLeft || isEndOfFlame == 0);
		
		FntPrint("%d\n", VSync(1));
		isEndOfFlame = 0;
			
		/* wait for V-BLNK*/
		VSync(0);
		
		/* swap disply buffer.
		 * Notice that drawing buffer (tranfering buffer) is
		 * always opposite one of displaying buffer. */
		id = dec.rectid? 0: 1;
		
		PutDispEnv(SetDefDispEnv(&disp, 0, id==0? 0:240, 256, 240));
		PutDrawEnv(SetDefDrawEnv(&draw, 0, id==0? 0:240, 256, 240));
		FntFlush(-1);
	}

	/* restore status */
	DrawSync(0);
	DecDCTinSync(0);
	DecDCToutCallback(0);
	DecDCTvlcSize2(0);
	return;
}

/*
 *	read next bitstream data (pseudo CD-ROM simulator)	 */
static int frame_no = 0;
static void StrRewind(void)
{
	frame_no = 0;
}

static u_long *StrNext(void)
{
	extern	u_long	*mdec_frame[];

	FntPrint("%4d: %4d byte\n",
		 frame_no,
		 mdec_frame[frame_no+1]-mdec_frame[frame_no]);
	
	if (mdec_frame[frame_no] == 0) 
		return(mdec_frame[frame_no = 0]);
	else
		return(mdec_frame[frame_no++]);
}

