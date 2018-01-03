/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *		      simple scroll backgournd
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00		1995/08/28	suzu
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define WIDTH	320		/* horizontal width (256/320/360/512/640) */
static u_short	tpage;		/* BG tpage */
static SPRT	sprt[2];	/* BG sprite */

/* animation pattern */
static int	boynum  = 16;	
static RECT	boytw[] = {
	{0,   0, 32, 32},{0,  32, 32, 32}, {0,   0, 32, 32}, {0,  32, 32, 32},
	{32,  0, 32, 32},{32, 32, 32, 32}, {32,  0, 32, 32}, {32, 32, 32, 32},
	{96,  0, 32, 32},{96, 32, 32, 32}, {96,  0, 32, 32}, {96, 32, 32, 32},
	{64,  0, 32, 32},{64, 32, 32, 32}, {64,  0, 32, 32}, {64, 32, 32, 32},
};


static int first = 1;

void bgFlush(void)
{
	first = 1;
}

void bgUpdate(u_char col)
{
	static int idx = 0;
	static DRAWENV	draw;
	static DISPENV	disp;
	int	vcount;
	
	if (first) {
		extern u_long	boytex[];	
		u_short	tpage, clut;
		int	i;
		first = 0;
		
		tpage = LoadTPage(boytex+0x80, 0, 0, 960, 0, 128, 64);
		clut  = LoadClut(boytex, 960, 128);
		
		SetDefDrawEnv(&draw, 0, 0, WIDTH, 240);
		SetDefDispEnv(&disp, 0, 0, WIDTH, 240);
		draw.tpage = tpage;
		
		for (i = 0; i < 2; i++) {
			SetSprt(&sprt[i]);		/* ID */
			setXY0(&sprt[i], 0, 0);		/* XY location */
			setUV0(&sprt[i], 0, 0);		/* UV location */
			setWH(&sprt[i], WIDTH, 240);	/* size */
			sprt[i].clut  = clut;		/* clut */
			TermPrim(&sprt[i]);		/* terminate */
		}
	}

	vcount = VSync(-1);
	idx = (idx+1)&0x01;
	
	setRGB0(&sprt[idx], col, col, col);		/* color */
	setUV0( &sprt[idx], vcount&~0x01, vcount);	/* UV */
		
	disp.disp.y = idx? 240: 0;			/* display position */
	draw.ofs[1] = draw.clip.y = idx? 0: 240;	/* drawing postiont */
	draw.tw     = boytw[(vcount>>4)&(boynum-1)];	/* texture window */

	PutDrawEnv(&draw);
	PutDispEnv(&disp);
	DrawOTag((u_long *)&sprt[idx]);
}





