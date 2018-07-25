/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto5: simplest sample
 *
 *	parallel execution of LoadImage() and DecDCTout() using callback.
 *
 :	LoadImage() と、DecDCTout() をインターリーブしてみる
 *	しかも、インターリーブをコールバックを使用してバックグラウンドで
 *	実行する。	
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

u_long mdec_bs[] = {
#include "siro.bs"
};

static void callback(void);
static init_graph(int width, int height);

main()
{
	int	i;
	int	width  = 256;
	int	height = 160;

	init_graph(width, height);		/* reset graphic system */
	DecDCTReset(0);				/* reset MDEC */
	PadInit(0);
	disp_mdec(mdec_bs, width, height);	/* display */
	while (PadRead(1) != PADselect);
	PadStop();
	StopCallback();
	return;
}

static u_long	mdec_rl[256*256];	/* run-level buffer */
static u_long	mdec_image[16*256/2];	/* image slice buffer */
	
static RECT	rect;
static int	slice, total;
static int	width, height;

disp_mdec(mdec_bs, _width, _height)
u_long	*mdec_bs;
int	_width, _height;
{
	width  = _width;
	height = _height;
	slice  = 16*height/2;
	total  = width*height/2;
	
	rect.w = 16;
	rect.h = height;
	rect.x = 0;
	rect.y = (240-height)/2;
	
	DecDCToutCallback(callback);	/* set callback */
	
 	DecDCTvlc(mdec_bs, mdec_rl);	/* decode VLC */
 	DecDCTin(mdec_rl, 0);		/* send run-level */
	DecDCTout(mdec_image, slice);	/* first out */
	DecDCTinSync(0);
}

static void callback(void)
{
	LoadImage(&rect, mdec_image);			/* load */
	if ((rect.x += 16) < width)
		DecDCTout(mdec_image, slice);		/* recieve */
	else 
		DecDCToutCallback(0);
}
	
static init_graph(int width, int height)
{
	DISPENV	disp;
	RECT	rect;
	
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	SetDefDispEnv(&disp, 0, 0, width, 240);	/* setup display environment */
	PutDispEnv(&disp);	
	
	/* clear flame buffer */
	rect.w = 640;	rect.h = 480;
	rect.x =   0;	rect.y =   0;
	ClearImage(&rect, 0, 0, 0);

}	
