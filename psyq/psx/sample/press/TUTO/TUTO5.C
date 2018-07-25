/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto5: simplest sample
 */
/*	parallel execution of LoadImage() and DecDCTout() using callback.
 * */
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

static void disp_mdec(u_long *mdec_bs, int w, int h, DECDCTTAB table);
static void callback(void);
static void init_graph(int width, int height);

void tuto5(void)
{
	int		i;
	int		width  = 256;
	int		height = 160;
	DECDCTTAB	table;
	
	init_graph(width, height);		/* reset graphic system */
	DecDCTvlcBuild(table);

	disp_mdec(mdec_bs, width, height, table);	/* display */
	while (PadRead(1) != PADselect);
	
	return;
}

static RECT	rect;
static int	slice, total;
static int	width, height;

static void disp_mdec(u_long *mdec_bs, int w, int h, DECDCTTAB table)
{
	width  = w;
	height = h;
	slice  = 16*height/2;
	total  = width*height/2;
	
	rect.w = 16;
	rect.h = height;
	rect.x = 0;
	rect.y = (240-height)/2;
	
	DecDCToutCallback(callback);		/* set callback */
 	DecDCTvlc2(mdec_bs, mdec_rl, table);	/* decode VLC */
 	DecDCTin(mdec_rl, 0);			/* send run-level */
	DecDCTout(mdec_image, slice);		/* first out */
	
	/* termination */
	DecDCTinSync(0);
	DecDCToutSync(0);
	DecDCToutCallback(0);
}

static void callback(void)
{
	LoadImage(&rect, mdec_image);			/* load */
	if ((rect.x += 16) < width)
		DecDCTout(mdec_image, slice);		/* recieve */
	else 
		DecDCToutCallback(0);
}
	
static void init_graph(int width, int height)
{
	DISPENV	disp;
	RECT	rect;
	
	/* setup display environment */
	SetDefDispEnv(&disp, 0, 0, width, 240);	
	PutDispEnv(&disp);	
	
	/* clear flame buffer */
	rect.w = 640;	rect.h = 480;
	rect.x =   0;	rect.y =   0;
	ClearImage(&rect, 0, 0, 0);

}	
