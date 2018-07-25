/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto3: simplest sample
 */
/*		simple on-memory movie operation (with	frame
 *		double-buffering)  */	
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

static void init_graph(int width,int  height);
static int disp_mdec(u_long *mdec_bs, DECDCTTAB table);

extern u_long mdec_image[];
extern u_long mdec_rl[];

void tuto3(void)
{
	extern u_long	*mdec_frame[];
	int		width = 256, height = 176;
	int		i, j;
	DECDCTTAB	table;
	
	init_graph(width, height);		/* reset graphic system */
	DecDCTvlcBuild(table);			/* generate VLC table */
	
	while (1) {
		for (i = 0; mdec_frame[i]; i++) 
			if (disp_mdec(mdec_frame[i], table) != 0) 
				return;
	}	
};

static int	Width, Height;			/* image size */
static int	SliceSize;			/* 1 slice data size */
static RECT	SliceRect;			/* slice rectangle */
static DISPENV	Disp[2];			/* display area */

static void init_graph(int width,int  height)
{
	RECT	rect;
	
	Width       = width;		/* set image width and height */
	Height      = height;		
	SliceSize   = 16*Height/2;	/* set slice image size (word) */
	
	SliceRect.w = 16;		/* set slice area width and height */
	SliceRect.h = Height;

	/* clear image */
	rect.w = 640;	rect.h = 480;
	rect.x =   0;	rect.y =   0;
	ClearImage(&rect, 0, 0, 0);
	
	/* set display area (for double buffer)
	 * #0: (0,  0)-(width, 240)  
	 * #1: (0,240)-(width, 480)  
	 */
	SetDefDispEnv(&Disp[0], 0, 240, Width, 240);	
	SetDefDispEnv(&Disp[1], 0,   0, Width, 240);
}	

static int disp_mdec(u_long *mdec_bs, DECDCTTAB table)
{
	static int	id = 0;			/* double buffer ID */


	DecDCTvlc2(mdec_bs, mdec_rl, table);	/* decode VLC */
	DecDCTin(mdec_rl, 0);			/* send run-level */

	SliceRect.x = 0;
	SliceRect.y = (id==0)? 240+(240-Height)/2: (240-Height)/2;
	
	for (SliceRect.x = 0; SliceRect.x < Width; SliceRect.x += 16) {
		DecDCTout(mdec_image, SliceSize);		/* recieve */
		DecDCToutSync(0);				/* wait */
		LoadImage(&SliceRect, mdec_image);		/* load */
	}
	
	if (PadRead(1)&PADselect) {
		DrawSync(0);
		return(-1);
	}
	
	VSync(0);
	PutDispEnv(&Disp[id]);
	id = (id==0)? 1: 0;
	return(0);
}

