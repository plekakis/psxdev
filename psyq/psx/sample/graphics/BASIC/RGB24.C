/* $PSLibId: Run-time Library Release 4.4$ */
/*			24bit: 24bit mode 
 *
 *		Copyright (C) 1993,1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Aug,31,1993	suzu
 *	2.00		Apr.28.1994	suzu	(rewrite)
 *	2.10		Mar.06.1997	sachiko	(added autopad)
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* for controller recorder */
#define PadRead(x)	myPadRead(x)

#define WIDTH	(64*3/2)
#define HEIGHT	32
#define MAXWALK	128
#define SPEED	4
#define SPEED2	60
#define UP	1
#define DOWN	2
#define RIGHT	3
#define LEFT	4

static int walk_forward(RECT *rect);
static int walk_backward(RECT *rect, int direc);
static int pause(int n);

void Rgb24(void)
{	
	static RECT src = {		/* rectangle for Store/Move/Load */
		640*3/2/2, 240/2, WIDTH, HEIGHT,
	};
	static int	walk[MAXWALK];
	static u_long	imgbuf[WIDTH*HEIGHT*3/4];
	
	DISPENV	disp;
	RECT	src_old;
	int	i, j, flag, direc, padd;
	
	expand_image24(640, 240);		/* load 24bit image data */ 
						  
	/* initialize flame buffer */
	SetDefDispEnv(&disp, 0, 0, 640, 240);	/* init DISPENV */
	disp.isrgb24 = 1;			/* 24bit mode */
	PutDispEnv(&disp);			/* set DISPENV to system */
	SetDispMask(1);				/* enable to display */

	memcpy(&src_old, &src, sizeof(RECT));	/* set first position */

	/* reset for while.. */
	if (pause(SPEED2)) goto finish;
	
	/* walk forward */
loop:	
	for (i = 0; i < MAXWALK; i++) {
		walk[i] = walk_forward(&src);		/* get next step */
		MoveImage(&src, src_old.x, src_old.y);	/* copy tile */
		ClearImage(&src, 0, 0, 0);	/* clear old tile */
		memcpy(&src_old, &src, sizeof(src));
		/*if (pause(SPEED)) goto finish; */
	}
	
	/* rest for while.. */
	if (pause(SPEED2)) goto finish;
	
	/* walk backward */
	for (i = MAXWALK-1; i >= 0; i--) {
		walk_backward(&src, walk[i]);	/* get previous step */
		
		StoreImage(&src,    imgbuf);	/* save tile */
		LoadImage(&src_old, imgbuf);	/* load tile */
		ClearImage(&src, 0, 0, 0);	/* clear old tile */
		memcpy(&src_old, &src, sizeof(src));
		if (pause(SPEED)) goto finish;
	}
	
	/* rest for while.. */
	if (pause(SPEED2)) goto finish;
	
	goto loop;
    finish:
	DrawSync(0);
	return;
}

#define MIN_H	HEIGHT
#define MAX_H	(240-HEIGHT*2)
#define MIN_W	WIDTH
#define MAX_W	(640*3/2-WIDTH*2)

static int walk_forward(RECT *rect)
{
	int		direc;
	static int	odirec = 0;
	
	while (1) {
		while (((direc = rand()%4)^odirec) == 1); 

		if (direc == 0 && rect->y > MIN_H) {		/* UP */
			rect->y -= HEIGHT; break;
		}
		if (direc == 1 && rect->y < MAX_H) {		/* DOWN */
			rect->y += HEIGHT; break;
		}
		if (direc == 2 && rect->x > MIN_W) {		/* LEFT */
			rect->x -= WIDTH; break;
		}
		if (direc == 3 && rect->x < MAX_W) {		/* RIGHT */
			rect->x += WIDTH; break;
		}
	}
	odirec = direc;
	return(direc);
	
}

static int walk_backward(RECT *rect, int direc)
{
	switch (direc) {
	    case 0:	rect->y += HEIGHT; break;
	    case 1:	rect->y -= HEIGHT; break;
	    case 2:	rect->x += WIDTH;  break;
	    case 3:	rect->x -= WIDTH;  break;
	}
	return(0);
}

static int pause(int n)
{
	while (n--) {
		VSync(0);
		if (PadRead(1) == PADselect)
			return(-1);
	}
	return(0);
}


u_long mdec_bs[] = {
#include "flower.bs"
};

#define PPW	3/2
expand_image24(width, height)
int	width;

int	height;
{
	RECT	rect;
	int	slice = 16*height*PPW/2;
	u_long	mdec_rl[256*256];	/* run-level buffer */
	u_short	mdec_image[16*240*PPW];	/* image slice buffer */
	
	DecDCTvlc(mdec_bs, mdec_rl);		/* decode VLC */
	DecDCTin(mdec_rl, 1);			/* send run-level */
	
	setRECT(&rect, 0, 0, 16*PPW, height);
	for (rect.x = 0; rect.x < width*PPW; rect.x += 16*PPW) {
		DecDCTout(mdec_image, slice);		/* recieve */
		DecDCToutSync(0);			/* wait */
		LoadImage(&rect, (u_long *)mdec_image);	/* load */
	}
}

static memcpy(p, q, n)
u_char	*p, *q;
int	n;
{
	while (n--) 
                *p++ = *q++;
}


