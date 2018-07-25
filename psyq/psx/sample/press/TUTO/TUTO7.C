/* $PSLibId: Run-time Library Release 4.4$ */
/*			  tuto7: simplest sample
 *	
 *		    Copyright (C) 1993 by Sony Corporation
 *			   All rights Reserved
 *
 *			    fine tune-up
 */
/*	DecDCTGetEnv() stores the internal environment of DCT decoding
 *	in DECDCTENV structure. Fine tune-up can be possible to change
 *	these parameters. For example, output color tone can be
 *	changed by touching DC value of Y(luminance) and
 *	C(chrominance) IQ block.
 *	See /usr/psx/doc/eng/format/mdec.txt for detail description
 *	 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

extern u_long mdec_image[];
extern u_long mdec_rl[];

static void init_graph(int width, int height);
static void disp_mdec(u_long *mdec_bs, DECDCTTAB table);
static int pad_read(void);

void tuto7(void)
{
	extern u_long	*mdec_frame[];
	int		width = 256, height = 176;
	int		i, j;
	DECDCTTAB	table;
	
	init_graph(width, height);		/* reset graphic system */
	DecDCTvlcBuild(table);			/* build VLC table */
	FntLoad(960, 256);			/* load Font */
	SetDumpFnt(FntOpen(16, 16, 256, 32, 1, 512));

	while ((PadRead(1)&PADselect) == 0) {
		for (i = 0; mdec_frame[i]; i++) {
			
			/* change environment */
			if (pad_read() == -1)
				break;

			/* display frame data */
			disp_mdec(mdec_frame[i], table);

			/* update strings */
			FntFlush(-1);
		}
	}
	DrawSync(0);
	return;
};

static int	Width, Height;			/* image size */
static int	SliceSize;			/* 1 slice data size */
static RECT	SliceRect;			/* slice rectangle */
static DISPENV	Disp[2];			/* display area */
static DRAWENV	Draw[2];			/* drawing area */

static void init_graph(int width, int height)
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
	SetDefDispEnv(&Disp[0], 0, 240, width, 240);	
	SetDefDrawEnv(&Draw[0], 0,   0, width, 240);	
	SetDefDispEnv(&Disp[1], 0,   0, width, 240);
	SetDefDrawEnv(&Draw[1], 0, 240, width, 240);
}	

static void disp_mdec(u_long *mdec_bs, DECDCTTAB table)
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
	VSync(0);

	PutDispEnv(&Disp[id]);
	PutDrawEnv(&Draw[id]);

	id = (id==0)? 1: 0;
}

static void display_image(u_long *addr, int width, int height)
{
	DISPENV	disp;
	DRAWENV	draw;
	RECT	rect;
	
	/* setup display environment */
	PutDispEnv(SetDefDispEnv(&disp, 0, 0, width, 240));	
	PutDrawEnv(SetDefDrawEnv(&draw, 0, 0, width, 240));	

	rect.w = 16;
	rect.h = height;
	rect.y = (240-height)/2;

	for (rect.x = 0; rect.x < width; rect.x += 16, addr += 8*height) 
		LoadImage(&rect, addr);
	DrawSync(0);
}

static int pad_read(void)
{
	static int	opadd = 0;
	DECDCTENV	env;
	int		padd = PadRead(1), is_change = 0;
	
	DecDCTGetEnv(&env);
	
	if (padd == PADselect)
		return(-1);
	
	if (opadd == 0 && (padd&PADLup))
		env.iq_y[0]++, is_change = 1;
	if (opadd == 0 && (padd&PADLdown))
		env.iq_y[0]--, is_change = 1;
	if (opadd == 0 && (padd&PADLright))
		env.iq_c[0]++, is_change = 1;
	if (opadd == 0 && (padd&PADLleft))
		env.iq_c[0]--, is_change = 1;
		
	opadd = padd;
	if (is_change) {
		limitRange(env.iq_y[0], 1, 64);
		limitRange(env.iq_c[0], 1, 64);
		DecDCTPutEnv(&env);
	}
	FntPrint("Push Left Cross Key...\n");
	FntPrint("Y=%d, C=%d\n",
		 env.iq_y[0], env.iq_c[0]);
	
	return(0);
}


