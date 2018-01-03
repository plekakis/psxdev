/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto4: simplest sample
 */
/*	handshake using callback.
 *	Callback is the function entry which is called when the end of
 *	background translation. When using callback, all movie decode
 *	can be done entirely in the background.
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

static int	total;			/* total decoded data size */
static int	is_infunc, is_outfunc;
static void	infunc(void), outfunc(void);
static void	display_image(u_long *addr, int width, int height);

void tuto4(void)
{
	int		width  = 256;
	int		height = 160;
	int		i, ret = 0;
	u_long		data;
	DECDCTTAB	table;
	
	total     = width*height/2;
	is_infunc = is_outfunc = 0;
	
	/* clear work space */
	for (i = 0; i < width*height/2; i++)
		mdec_image[i] = 0;
	
	display_image(mdec_image, width, height);	/* display */

	DecDCTinCallback(infunc);		/* set callbacks */
	DecDCToutCallback(outfunc);		/* set callbacks */
	DecDCTvlcBuild(table);			/* build VLC table */
 	DecDCTvlc2(mdec_bs, mdec_rl, table);	/* decode VLC */
	DecDCTin(mdec_rl, 0);			/* send 1st encoded image */
	DecDCTout(mdec_image, total);		/* recive 1st decoded image */
	DecDCToutSync(0);

	display_image(mdec_image, width, height);	/* display */
	printf("intr=(%d,%d), ret = %d\n", is_infunc, is_outfunc, ret);
	
	while ((PadRead(1)&PADselect) == 0);
	
	DrawSync(0);
	DecDCToutSync(0);
	DecDCTinSync(0);
	DecDCToutCallback(0);
	DecDCTinCallback(0);
	return;
}

static void display_image(u_long *addr, int width, int height)
{
	DISPENV	disp;
	RECT	rect;
	
	/* clear flame buffer */
	rect.w = 640;	rect.h = 480;
	rect.x =   0;	rect.y =   0;
	ClearImage(&rect, 0, 0, 0);

	/* setup display environment */
	SetDefDispEnv(&disp, 0, 0, width, 240);	
	PutDispEnv(&disp);	
	
	rect.w = 16;
	rect.h = height;
	rect.y = (240-height)/2;
	
	for (rect.x = 0; rect.x < width; rect.x += 16, addr += 8*height) 
		LoadImage(&rect, addr);
	DrawSync(0);
}

static void infunc(void)
{
	is_infunc++;
	DecDCTinCallback(0);
	DecDCToutSync(0);
	DecDCTin(mdec_rl, 0);
}

static void outfunc(void)
{
	is_outfunc++;
	DecDCToutCallback(0);
	DecDCTout(mdec_image, total);
}
