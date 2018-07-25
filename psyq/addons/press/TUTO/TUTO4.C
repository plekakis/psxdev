/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto4: simplest sample
 *
 *	handshake using callback.
 *	Callback is the function entry which is called when the end of
 *	background translation. When using callback, all movie decode
 *	can be done entirely in the background.
 *	
 :	コールバックの実現
 *	コールバックは、ノンブロック関数の終了時に呼ばれる関数のポイン
 *	タです。コールバックを有効に使用することによって動画再生の処理
 *	を完全にバックグラウンドで実行することができます。
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

u_long	mdec_rl[256*256];	/* run-level buffer */
u_long	mdec_image[256*160];	/* decoded data buffer #0 */
int	total;			/* total decoded data size */
int	is_infunc, is_outfunc;
void	infunc(), outfunc();

main()
{
	int	width  = 256;
	int	height = 160;
	int	i, ret = 0;
	u_long	data;
	
	total = width*height/2;
	
	is_infunc = is_outfunc = 0;
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	DecDCTReset(0);		/* reset MDEC */
	PadInit(0);
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	/* clear work space */
	for (i = 0; i < width*height/2; i++)
		mdec_image[i] = 0;
	
	display_image(mdec_image, width, height);	/* display */

	DecDCTinCallback(infunc);		/* set callbacks */
	DecDCToutCallback(outfunc);		/* set callbacks */
	
 	DecDCTvlc(mdec_bs, mdec_rl);		/* decode VLC */
	DecDCTin(mdec_rl, 0);			/* send 1st encoded image */
	DecDCTout(mdec_image, total);		/* recive 1st decoded image */
	DecDCToutSync(0);

	display_image(mdec_image, width, height);	/* display */
	
	printf("intr=(%d,%d), ret = %d\n", is_infunc, is_outfunc, ret);
	
	while (PadRead(1) != PADselect);
	PadStop();
	StopCallback();
	return(ret);
}

display_image(addr, width, height)
u_long	*addr;
int	width, height;
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

void infunc(void)
{
	is_infunc++;
	DecDCTinCallback(0);
	DecDCToutSync(0);
	DecDCTin(mdec_rl, 0);
}

void outfunc(void)
{
	is_outfunc++;
	DecDCToutCallback(0);
	DecDCTout(mdec_image, total);
}
