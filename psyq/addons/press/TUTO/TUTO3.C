/* $PSLibId: Runtime Library Release 3.6$ */
/*			tuto3: simplest sample
 *
 *		simple on-memory movie operation (with	frame
 *		double-buffering) 
 :		単純な動画の再生（フレームダブルバッファ付き）
 *	
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

main()
{
	extern u_long	*mdec_frame[];
	int		width = 256, height = 176;
	int		i, j;
	
	init_graph(width, height);		/* reset graphic system */
	DecDCTReset(0);				/* reset MDEC */
	PadInit(0);

	while ((PadRead(1)&PADselect) == 0) {
		for (i = 0; mdec_frame[i]; i++) 
			disp_mdec(mdec_frame[i], width, height);
	}	
	PadStop();
	StopCallback();
	return;
};

static int	Width, Height;			/* image size */
static int	SliceSize;			/* 1 slice data size */
static RECT	SliceRect;			/* slice rectangle */
static DISPENV	Disp[2];			/* display area */

init_graph(width, height)
int	width, height;
{
	RECT	rect;
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
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
	SetDefDispEnv(&Disp[1], 0,   0, width, 240);
}	

disp_mdec(mdec_bs)
u_long	*mdec_bs;
{
	static u_long	mdec_rl[256*256];	/* run-level buffer */
	static u_long	mdec_image[16*256/2];	/* image slice buffer */
	static int	id = 0;			/* double buffer ID */
	
	DecDCTvlc(mdec_bs, mdec_rl);		/* decode VLC */
	DecDCTin(mdec_rl, 0);		/* send run-level */

	SliceRect.x = 0;
	SliceRect.y = (id==0)? 240+(240-Height)/2: (240-Height)/2;
	
	for (SliceRect.x = 0; SliceRect.x < Width; SliceRect.x += 16) {
		DecDCTout(mdec_image, SliceSize);		/* recieve */
		DecDCToutSync(0);				/* wait */
		LoadImage(&SliceRect, mdec_image);		/* load */
	}
	VSync(0);
	PutDispEnv(&Disp[id]);

	id = (id==0)? 1: 0;
}
