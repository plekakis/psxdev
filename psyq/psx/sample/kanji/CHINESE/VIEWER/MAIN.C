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
#include <strings.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

/* Now testing... */
/*
#define DEBUG					/* debugging now */

/* defines */
#define FONTADDR	0x80010000		/* font data load address */
#define TBLADDR		0x80050000		/* code table load address */

#define SWIDTH		384			/* screen width */
#define SHEIGHT		240			/* screen height */

#define KW		16			/* number of kanji(x) */
#define KH		12			/* number of kanji(y) */
#define VW		17			/* kanji font size(yoko) */
#define VH		17			/* kanji font size(tate) */
#define STARTX		14			/* KanjiPrint start address x */
#define STARTY		12			/* KanjiPrint start address y */
#define WX		((VW*4)+STARTX)		/* wall address x */
#define WY		(VH+STARTY)		/* wall address y */
#define WW		KW*VW			/* wall width */
#define WH		KH*VW			/* wall height */
#define BUFSIZE		VW*VH			/* buffer size */
#define COLOR		0x7fff			/* kanji fg */
#define BLACK		0x3000			/* kanji bg */
#define STEPMAX		30			/* padread speed up count */
#define KFONTH		15			/* kanji font size (height) */
#ifdef HALF
#define KSIZE		KFONTH			/* kanji font data size */
#define KFONTW		8			/* kanji font size (width) */
#define MAXKNUM		157			/* max number of kanji */
#define MAXSCODE	160			/* max code No. */
#else /* HALF */
#define KSIZE		KFONTH*2		/* kanji font data size */
#define KFONTW		15			/* kanji font size (width) */
#define MAXKNUM		8099			/* max number of kanji */
#define MAXSCODE	8112			/* max code No. */
#endif /* HALF */
#define MINSCODE	0			/* min code No. */
#define OTSIZE		1			/* depth of OT */
#define FNTSIZE		KFONTW*KFONTH		/* display font size */
#define FNTW		8			/* display font width */ 
#define FNTH		8			/* display font height */ 

#define MAXCODETBL	8091			/* max number of code table */
#define TW		(KFONTW*(FNTW+1)+1)	/* font tile width */
#define TH		(KFONTH*(FNTH+1)+1)	/* font tile height */
#define TX		((SWIDTH-TW)/2)		/* font tile start position x */
#define TY		((SHEIGHT-TH)/2)	/* font tile start position y */

#define FX		TX			/* debug font position x */
#define FY		(TY+TH+4)		/* debug font position y */
#define FW		TW			/* debug font width */
#define FH		24			/* debug font height */

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ot */
	SPRT_16		sprt0[KW];		/* sprt */
	SPRT_16		sprt1[KH*4];		/* sprt */
	TILE_16		cursor;			/* cursor */
	TILE		tile;			/* tile */		
	TILE		font[FNTSIZE];		/* font */
} DB;

typedef struct {
	u_short		font;
	char		pad0[2];
	char		cxcode[4];
	char		pad1[2];
	char		sjcode[4];
	char		pad2[2];
} CODETBL;

typedef struct {
	int	x;
	int	y;
} POS;

POS cur_pos = {0, 0};		/* cursor position */
int cur_scode = 0;		/* current code */
u_short cur_fg = COLOR;		/* current fg */
u_short cur_bg = BLACK;		/* current bg */
int dflag = 0;			/* display flag */

/* prototype */
static void initTexture(u_short scode);
static void _get_font(u_short *data, u_char *addr);
static int pad_read(void);
static void init_num();
static void init_prim(DB *db);
static void get_tile(TILE *tp);

DB	db[2];		/* packet double buffer */
DB	*cdb;		/* current db */

/*
 * main routine
 */
main()
{
	CODETBL	*ctbl = (CODETBL *)TBLADDR;
	int	i, j;

	ResetCallback();
	PadInit(0);             /* initialize PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,       0, SWIDTH, SHEIGHT);	
	SetDefDrawEnv(&db[1].draw, 0, SHEIGHT, SWIDTH, SHEIGHT);
	SetDefDispEnv(&db[0].disp, 0, SHEIGHT, SWIDTH, SHEIGHT);
	SetDefDispEnv(&db[1].disp, 0,       0, SWIDTH, SHEIGHT);

	init_prim(&db[0]);
	init_prim(&db[1]);
	
	/* initialize Kanji Font */
	init_num();

	/* initialize debug font */
	FntLoad(960, 0);
	FntOpen(FX, FY, FW, FH, 1, 256);

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while (pad_read() == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */

		ClearOTag(cdb->ot, OTSIZE);

		if (dflag) {
			TILE	*tp = cdb->font;
			char	buf[5];
			int	id = cur_scode+cur_pos.x+cur_pos.y*KW;

			get_tile(tp);
			for (i = 0; i < FNTSIZE; i++, tp++) {
				AddPrim(cdb->ot, tp);
			}
			AddPrim(cdb->ot, &cdb->tile);

#ifndef HALF
			if (id > MAXCODETBL)	id = MAXCODETBL;

			FntPrint("CODE NO\n");
			buf[4] = '\0';
			strncpy(buf, ctbl[id].sjcode, 4);
			FntPrint("SJIS  : %s\n", buf);
			strncpy(buf, ctbl[id].cxcode, 4);
			FntPrint("CX(GB): %s\n", buf);
#endif /* !HALF */
		}

		for (i = 0; i < KW; i++) {
			AddPrim(cdb->ot, &cdb->sprt0[i]);
		}
		for (i = 0; i < KH; i++) {
			int	cd = cur_scode+i*16;

			for (j = 0; j < 4; j++) {
				SPRT_16	*sp = &cdb->sprt1[i*4+j];

				setUV0(sp, (cd%16)*16, 0);
				AddPrim(cdb->ot, sp);
				cd >>= 4;
			}
		}

		setXY0(&cdb->cursor, WX+cur_pos.x*VW, WY+cur_pos.y*VH);
		AddPrim(cdb->ot, &cdb->cursor);

		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */

		initTexture(cur_scode);

		DrawOTag(cdb->ot);

#ifndef HALF
		if (dflag) {
			FntFlush(-1);
		}
#endif /* !HALF */
	}
        PadStop();
	ResetGraph(3);		/* reset graphic subsystem (0:cold,1:warm) */
	StopCallback();
	return 0;
}

/*
 * pad read routine
 */
static int pad_read(void)
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
	}
	if (padd & PADLdown) {
		cur_scode -= step;

		if (cur_scode < MINSCODE)
			cur_scode = MINSCODE;
	}
	if (padd & PADRup) {
		if (cur_pos.y > 0) 
			cur_pos.y--;
	}
	if (padd & PADRdown) {
		if (cur_pos.y < KH-1)
			cur_pos.y++;
	}
	if (padd & PADRright) {
		if (cur_pos.x < KW-1)
			cur_pos.x++;
	}
	if (padd & PADRleft) {
		if (cur_pos.x > 0)
			cur_pos.x--;
	}
	if (padd & (PADL1|PADL2|PADR1|PADR2)) {
		dflag = 1;
	} else {
		dflag = 0;
	}

	if ((padd & PADstart) && (padd & PADselect)) 	ret = -1;

	oldpadd = padd;

	return(ret);
}		

static void init_prim(DB *db)
{
	DR_ENV	*dp;
	DRAWENV	env;
	int	i, j;

	db->draw.isbg = 1;
	db->draw.tpage = getTPage(0, 0, 640,0);
	setRGB0(&db->draw, 60, 120, 120);

	{
		TILE_16	*tp = &db->cursor;

		setTile16(tp);
		setRGB0(tp, 255, 0, 0);
		setSemiTrans(tp, 1);
	}

	for (i = 0; i < KW; i++) {
		SPRT_16	*sp = &db->sprt0[i];
		
		setSprt16(sp);
		setXY0(sp, WX+i*VW, WY-VH);
		setUV0(sp, i*16, 0);
		setShadeTex(sp, 1);
		sp->clut = getClut(0, 480);	
	}
	for (i = 0; i < KH; i++) {
		for (j = 0; j < 4; j++) {
			SPRT_16	*sp = &db->sprt1[i*4+j];
		
			setSprt16(sp);
			setXY0(sp, STARTX+(3-j)*VW, WY+i*VH);
			setShadeTex(sp, 1);
			sp->clut = getClut(0, 480);	
		}
	}
	for (i = 0; i < FNTSIZE; i++) {
		TILE *tpp = &db->font[i];

		setTile(tpp);
		setRGB0(tpp, 200, 200, 200);
		setSemiTrans(tpp, 0);
		setWH(tpp, FNTW, FNTH);
		setXY0(tpp, (TX+1)+(i%KFONTW)*(FNTW+1), 
			    (TY+1)+(i/KFONTW)*(FNTH+1));
	}
	{
		TILE *tpp = &db->tile;

		setTile(tpp);
		setRGB0(tpp, 0, 0, 0);
		setSemiTrans(tpp, 0);
		setWH(tpp, TW, TH);
		setXY0(tpp, TX, TY);
	}
}

/*
 * initialize kanji data texture
 */
static void initTexture(u_short scode)
{
	RECT	rect;
	u_short	buf[BUFSIZE];
	int	i, j;
	u_char	*kaddr;

	/* load kanji data */
	for (j = 0; j < KH; j++) {
		for (i = 0; i < KW; i++) {
			int code = scode+j*KW+i;
			
			if (code >= MAXKNUM) {
				kaddr = (u_char *)FONTADDR;
			} else {
				kaddr = (u_char *)FONTADDR + code*KSIZE;
			}
			DrawSync(0);
			_get_font(buf, kaddr);
        		/* send pixel data to VRAM */
        		rect.x = WX+i*VW;
        		rect.y = WY+j*VH + cdb->draw.ofs[1];
        		rect.w = VW;
        		rect.h = VH;
        		LoadImage(&rect, (u_long *)buf);
		}
	}
}

/*
 * convert kanji data to 16bit direct
 */
static void _get_font(u_short *data, u_char *addr)
{
	u_short	*d;
	u_char	*p;
	int	i, j;

	p = (u_char *)addr;
	d = data;
	for (i = 0; i < KFONTH; i++) {
		for (j = 7; j >= 0; j--)
			*d++ = (((*p>>j)&0x01)==0x01)?cur_fg:cur_bg;
		p++;
#ifndef HALF
		for (j = 7; j >= 1; j--)
			*d++ = (((*p>>j)&0x01)==0x01)?cur_fg:cur_bg;
		p++;
#endif /* !HALF */
		for (j = 0; j < (VW-KFONTW); j++)
			*d++ = cur_bg;
	}
	for (i = 0; i < (VH-KFONTH); i++) {
		for (j = 0; j < VW ; j++) {
			*d++ = cur_bg;
		}
	}
}

static void init_num()
{
	u_long		kbuf[1024];
	TIM_IMAGE	image;

	Krom2Tim("0123456789ABCDEF", kbuf, 640, 0, 0, 480, 0x7fff, 0x0000);

	OpenTIM(kbuf);
	ReadTIM(&image);

	LoadImage(image.prect, image.paddr);
	LoadImage(image.crect, image.caddr);
}

static void get_tile(TILE *tp)
{
	u_char	*p;
	int	code;
	int	i, j;

	code = (cur_scode+(cur_pos.x+cur_pos.y*KW));
	if (code >= MAXKNUM) {
		p = (u_char *)FONTADDR;
	} else {
		p = (u_char *)FONTADDR + code*KSIZE;
	}
	for (i = 0; i < KFONTH; i++) {
		for (j = 7; j >= 0; j--, tp++) {
			if (((*p>>j)&0x01) == 0x01) {
				setRGB0(tp, 0, 0, 0);
			} else {
				setRGB0(tp, 255, 255, 255);
			}
		}
		p++;
#ifndef HALF
		for (j = 7; j >= 1; j--, tp++) {
			if (((*p>>j)&0x01) == 0x01) {
				setRGB0(tp, 0, 0, 0);
			} else {
				setRGB0(tp, 255, 255, 255);
			}
		}
		p++;
#endif /* !HALF */
	}
}
