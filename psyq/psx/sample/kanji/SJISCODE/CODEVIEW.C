/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: viewer.c
 *
 *      Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *                                        All Rights Reserved.
 *
 *      Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

/* Now testing... */
/*
#define DEBUG					/* debugging now */

/* kanji defines */
#define MINSCODE	0x8140			/* min sjis code */
#define MAXSCODE	0x9870			/* max sjis code */
#define KFONT		15			/* kanji font size(11/13/15) */
#define KW		16			/* number of kanji(x) */
#define KH		13			/* number of kanji(y) */
#define VW		16			/* kanji font size(yoko) */
#define VH		KFONT			/* kanji font size(tate) */
#define STARTX		16			/* KanjiPrint start address x */
#define STARTY		8			/* KanjiPrint start address y */
#define WX		64+STARTX		/* wall address x */
#define WY		16+STARTY		/* wall address y */
#define WW		KW*VW			/* wall width */
#define WH		KH*VW			/* wall height */
#define BUFSIZE		16*(KFONT+1)		/* buffer size */
#define COLOR		0x7fff			/* kanji texture fg */
#define BLACK		0x3000			/* kanji texture bg */
#define STEPMAX		30			/* padread speed up count */

#define NTSC_MODE       0
#define PAL_MODE        1

/* defines */
#define OTSIZE		16			/* ordering table size */

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_FT4	wall;			/* wall primitive */
} DB;

u_short	cur_scode = MINSCODE;	/* current sjis code */
u_short cur_fg = COLOR;		/* current fg */
u_short cur_bg = BLACK;		/* current bg */

/* prototype */
static pad_read();
static init_prim(DB *db);

/*
 * main routine
 */
main()
{
	DB	db[2];		/* packet double buffer */
	DB	*cdb;		/* current db */
	
	int	i;

	SetVideoMode( NTSC_MODE );   /* NTSC_MODE */
/*	SetVideoMode( PAL_MODE  );   /* PAL_MODE  */

	ResetCallback();
	PadInit(0);             /* initialize PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(1);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 368, 240);	
	SetDefDrawEnv(&db[1].draw, 0, 240, 368, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 368, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 368, 240);
	
	if ( GetVideoMode() ) {
	    db[0].disp.screen.y = db[1].disp.screen.y = 28;
	}

	/* initialize texture */
	initTexture(cur_scode);

	/* initialize primitives */
	init_prim(&db[0]);
	init_prim(&db[1]);

	/* initialize Kanji Font */
	KanjiFntOpen(STARTX, STARTY, 360, 240, 960, 0, 960, 256, 0, 256);

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while (pad_read() == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */

		AddPrim(cdb->ot, &cdb->wall);

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */

		DrawOTag(cdb->ot);	/* draw */

		KanjiFntPrint("    0123456789ABCDEF\n");
		for (i = 0; i < KH; i++)
			KanjiFntPrint("%04x\n", cur_scode+i*KW);
		KanjiFntFlush(-1);

	}
        PadStop();
	ResetGraph(3);		/* reset graphic subsystem (0:cold,1:warm) */
	StopCallback();
	return 0;
}

/*
 * pad read routine
 */
static pad_read()
{
	static u_long	oldpadd = 0;
	static u_long	step_cnt = 0;

	int	ret = 0;	
	int	step = KW;
	int	stepc = 1;
	u_long	padd = PadRead(1);
	
	if (padd == oldpadd) {
		if (++step_cnt > STEPMAX) {
			if (step_cnt > STEPMAX*2) {
				step = KW*4;
				stepc = 4;
			} else {
				step = KW*2;
				stepc = 2;
			}
		}		
	} else {
		step_cnt = 0;
	}

	if (padd & PADLup) {
		cur_scode += step;

		if (cur_scode > MAXSCODE)
			cur_scode = MAXSCODE;
		initTexture(cur_scode);		
	}
	if (padd & PADLdown) {
		cur_scode -= step;

		if (cur_scode < MINSCODE)
			cur_scode = MINSCODE;
		initTexture(cur_scode);		
	}
	if (padd & PADL1) {
		cur_fg = (cur_fg+stepc)&0x7fff;
		initTexture(cur_scode);		
	}
	if (padd & PADL2) {
		cur_fg = (cur_fg-stepc)&0x7fff;
		initTexture(cur_scode);		
	}
	if (padd & PADR1) {
		cur_bg = (cur_bg+stepc)&0x7fff;
		initTexture(cur_scode);		
	}
	if (padd & PADR2) {
		cur_bg = (cur_bg-stepc)&0x7fff;
		initTexture(cur_scode);		
	}
	if (padd & PADk) 	ret = -1;

	oldpadd = padd;

	return(ret);
}		

/*
 * initialize primitives
 */
static init_prim(db)
DB	*db;
{
	POLY_FT4	*sp;

	/* initialize bg */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* initialize wall */
	sp = &db->wall;
	SetPolyFT4(sp);
	sp->tpage = GetTPage(2, 0, 640, 0);
	setUVWH(sp, 0, 0, VW*KW-1, (VH+1)*KH-1);
	setXYWH(sp, WX, WY, WW, WH);
	SetSemiTrans(sp, 0);
	setRGB0(sp, 0x80, 0x80, 0x80);
}

/*
 * initialize kanji data texture
 */
initTexture(scode)
u_short scode;
{
	RECT rect;
	u_short buf[BUFSIZE];
	int i, j;
	u_long kaddr;

	DrawSync(0);

	/* load kanji data */
	for (j = 0; j < KH; j++) {
		for (i = 0; i < KW; i++) {
			kaddr = Krom2RawAdd2(scode+j*KW+i);
			_get_font(buf, kaddr);
        		/* send pixel data to VRAM */
        		rect.x = 640+i*VW;
        		rect.y = j*(VH+1);
        		rect.w = VW;
        		rect.h = VH+1;
        		LoadImage(&rect, (u_long *)buf);
		}
	}
}

/*
 * convert kanji data to 16bit direct
 */
_get_font(data, addr)
u_short *data;
u_long *addr;
{
	u_short *p, *d, w;
	long i, j;

	p = (unsigned short *)addr;
	d = data;
	for (i = 0; i < KFONT; i++) {
		for (j = 7; j >= 0; j--)
			*d++ = (((*p>>j)&0x01)==0x01)?cur_fg:cur_bg;
		for (j = 15; j >= 8; j--)
			*d++ = (((*p>>j)&0x01)==0x01)?cur_fg:cur_bg;
		p++;
	}
	for (i = 0; i < 16 ; i++) {
		*d++ = cur_bg;
	}
}
