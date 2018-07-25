/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto1: simplest sample
 */
/*	     simple VLC decode and MDEC on memory decompression */	
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
 * If you want to know current DCT coefficients, define this  */
/*#define PRINT_COEF*/

/*
 * Image is compressed as BitStream(BS) format, Notice that BS does
 * not have information about image size. */
extern u_long mdec_bs[];
extern u_long mdec_image[];
extern u_long mdec_rl[];

void display_image(u_long *addr, int width, int height);
void tuto1(void)
{
	int	width  = 256;
	int	height = 160;
	int	total  = width*height/2;
	int	i;
	DECDCTENV	env;
	DECDCTTAB	table;
	
#ifdef PRINT_COEF
	
	/* printout current (default) environment) */
	DecDCTGetEnv(&env);
	printf("\n\nIQ(Y)\n");
	for (i = 0; i < 64; i++)	printf("%02x  ", env.iq_y[i]);
	
	printf("\n\nIQ(C)\n");
	for (i = 0; i < 64; i++)	printf("%02x  ", env.iq_c[i]);
	
	/*
	 * To control brightness, change DC coef. of IQ table. */
	/*env.iq_y[0] = env.iq_c[0] = 4;*/
	DecDCTPutEnv(&env);
	
#endif
	for (i = 0; i < total; i++)		/* clear output area */
		mdec_image[i] = 0;
	
	DecDCTvlcBuild(table);			/* expand table */
	DecDCTvlc2(mdec_bs, mdec_rl, table);	/* deocde VLC */
	
	DecDCTin(mdec_rl, 0);			/* send run-level */
	DecDCTout(mdec_image, total);		/* recieve data */
	DecDCToutSync(0);				/* wait */

	display_image(mdec_image, width, height);	/* display */

	while (PadRead(0) != PADselect);
	return;
}

void display_image(u_long *addr, int width, int height)
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

