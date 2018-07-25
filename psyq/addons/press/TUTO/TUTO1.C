/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto1: simplest sample
 *
 *	     simple VLC decode and MDEC on memory decompression
 :	     単純な VLC デコードと MDEC オンメモリデコードの実験
 *	
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

/*
 * If you want to know current DCT coefficients, define this 
 : もしも現在使用している DCT の係数を知りたければこれを定義する
 */
/*#define PRINT_COEF*/

/*
 * Image is compressed as BitStream(BS) format, Notice that BS does
 * not have information about image size.
 : 画像データはビットストリーム (BS) 形式に圧縮される。BS は画像のサイ
 * ズに関する情報を持っていないことに注意	
 */
u_long mdec_bs[] = {
#include "siro.bs"
};

u_long mdec_image[256*256];
u_long mdec_rl[256*256];

main()
{
	int	width  = 256;
	int	height = 160;
	int	total  = width*height/2;
	int	i;
	DECDCTENV	env;
	
	PadInit(0);
	
#ifdef PRINT_COEF
	
	/* printout current (default) environment) */
	DecDCTGetEnv(&env);
	printf("\n\nIQ(Y)\n");
	for (i = 0; i < 64; i++)	printf("%02x  ", env.iq_y[i]);
	
	printf("\n\nIQ(C)\n");
	for (i = 0; i < 64; i++)	printf("%02x  ", env.iq_c[i]);
	
	/*
	 * To control brightness, change DC coef. of IQ table.
	 : IQ テーブルの DC 成分を変化させることで輝度の調整ができる
	 */
	/*env.iq_y[0] = env.iq_c[0] = 4;*/
	DecDCTPutEnv(&env);
	
#endif
	for (i = 0; i < total; i++)		/* clear output area */
		mdec_image[i] = 0;
	
	DecDCTReset(0);				/* reset MDEC */
	DecDCTvlc(mdec_bs, mdec_rl);		/* deocde VLC */
	DecDCTin(mdec_rl, 0);			/* send run-level */
	DecDCTout(mdec_image, total);		/* recieve data */
	DecDCToutSync(0);				/* wait */

	display_image(mdec_image, width, height);	/* display */

	while (PadRead(0) != PADselect);

	PadStop();
	StopCallback();
	return;
}

display_image(addr, width, height)
u_long	*addr;
int	width, height;
{
	DISPENV	disp;
	RECT	rect;
	
	ResetGraph(0);
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
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

