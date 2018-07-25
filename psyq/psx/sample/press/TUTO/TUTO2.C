/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto2: simplest sample
 */
/*	Paralell execution of LoadImage() and DecDCTout().
 *	LoadImage is always faster than DecDCTout, so decoded image
 *	translation works without double-buffering.
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

extern u_long mdec_bs[];
extern u_long mdec_image[];
extern u_long mdec_rl[];

static void init_graph(int width, int height);
static void disp_mdec(u_long *mdec_bs, int width, int height);

void tuto2(void)
{
	int	i;
	int	width  = 256;
	int	height = 160;

	init_graph(width, height);		/* reset graphic system */
	disp_mdec(mdec_bs, width, height);	/* display */
	while (PadRead(0) != PADselect);

	return;
}

static void disp_mdec(u_long *mdec_bs, int width, int height)
{
	
	DECDCTTAB	table;
	RECT		rect;
	int		slice = 16*height/2;
	int		total = width*height/2;
	
	rect.w = 16;
	rect.h = height;
	rect.y = (240-height)/2;
	
	DecDCTvlcBuild(table);
	DecDCTvlc2(mdec_bs, mdec_rl, table);	/* decode VLC */
	DecDCTin(mdec_rl, 0);			/* send run-level */

	for (rect.x = 0; rect.x < width; rect.x += 16) {
		DecDCTout(mdec_image, slice);		/* recieve */
		DecDCToutSync(0);			/* wait */
		LoadImage(&rect, mdec_image);		/* load */
	}
}

static void init_graph(int width, int height)
{
	DISPENV	disp;
	RECT	rect;
	
	SetDefDispEnv(&disp, 0, 0, width, 240);	/* setup display environment */
	PutDispEnv(&disp);	

	rect.w = 640;	rect.h = 480;
	rect.x =   0;	rect.y =   0;
	ClearImage(&rect, 0, 0, 0);
	
}	

