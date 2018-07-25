/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: main.c
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
#include <libgs.h>
#include <libetc.h>

/* Now testing... */
/*
#define DEBUG					/* debugging now */

/* kanji defines */
#define KADDR		0x80010000		/* kanji data address */
#define KFONT		15			/* kanji font size(11/13/15) */
#define KW		16			/* number of kanji(x) */
#define KH		16			/* number of kanji(y) */
#define VX		640			/* texture VRAM address x */
#define VY		0			/* texture VRAM address y */
#define VW		16			/* kanji font size(yoko) */
#define VH		KFONT			/* kanji font size(tate) */
#define BUFSIZE		16*(KFONT+1)		/* buffer size */
#define COLOR		0x4210			/* kanji texture fg */
#define BLACK		0x3000			/* kanji texture bg */

/* defines */
#define DOUBLE					/* screen size 640x480 */
#define OTSIZE		16			/* ordering table size */
#define SCR_Z		512			/* ordering table size */
#define XXX		KW*VW/2			/* x zahyou */
#define YYY		KH*(VH+1)/2			/* y zahyou */

#ifdef DOUBLE
#define WIDTH		640			/* screen width */
#define HEIGHT		480			/* screen height */
#else /* DOUBLE */
#define WIDTH		320			/* screen width */
#define HEIGHT		240			/* screen height */
#endif /* DOUBLE */

typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_FT4	wall;			/* wall primitive */
} DB;

/* prototype */
static pad_read();
static init_prim(DB *db);

/*
 * main routine
 */
main()
{
	SVECTOR x[4];		/* wall's position */
	DB	db[2];		/* packet double buffer */
	DB	*cdb;		/* current db */
	
	int	i, j;
	long	dmy, flg;

	ResetCallback();
	SetVideoMode(MODE_NTSC); /* NTSC mode */
	/* SetVideoMode(MODE_PAL); /* PAL mode */
	
	PadInit(0);             /* initialize PAD */
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(1);	/* set debug mode (0:off, 1:monitor, 2:dump) */
	
	setVector(&x[0], -XXX, -YYY, 0);
	setVector(&x[1],  XXX, -YYY, 0);
	setVector(&x[2], -XXX,  YYY, 0);
	setVector(&x[3],  XXX,  YYY, 0);

	InitGeom();			/* initialize geometry subsystem */
	SetGeomOffset(WIDTH/2, HEIGHT/2); /* set geometry origin */	
	SetGeomScreen(SCR_Z);		/* distance to viewing-screen */

	/* initialize environment for double buffer */
#ifdef DOUBLE
	SetDefDrawEnv(&db[0].draw, 0,   0, 640, 480);	
	SetDefDrawEnv(&db[1].draw, 0,   0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0,   0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0,   0, 640, 480);
#else /* DOUBLE */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);	
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);
#endif /* DOUBLE */
	
	/* initialize texture */
	initTexture(KADDR);

	/* initialize primitives */
	init_prim(&db[0]);
	init_prim(&db[1]);

	/* display */
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */
	
	while (pad_read() == 0) {
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */

		RotTransPers4(&x[0], &x[1], &x[2], &x[3],
			(long *)&cdb->wall.x0, (long *)&cdb->wall.x1,
			(long *)&cdb->wall.x2, (long *)&cdb->wall.x3,
			&dmy, &flg);
		AddPrim(cdb->ot, &cdb->wall);

		/* swap buffer */
		DrawSync(0);	/* wait for end of drawing */
		VSync(0);	/* wait for the next V-BLNK */
	
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		PutDispEnv(&cdb->disp); /* update display environment */
		/*DumpOTag(cdb->ot);	/* for debug */
		DrawOTag(cdb->ot);	/* draw */
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
	static SVECTOR	ang = {0, 0, 0};	/* rotate angle */
	static VECTOR	vec = {0, 0, SCR_Z};	/* translage vector */
	static MATRIX	m;			/* matrix */

	int	ret = 0;	
	u_long	padd = PadRead(1);
	
	if (padd & PADn)	vec.vz += 8;
	if (padd & PADl) {
		vec.vz -= 8;
		if (vec.vz < 0)
			vec.vz = 0;
	}
	if (padd & PADLup) {
		vec.vy -= 8;
		if (vec.vy < -HEIGHT/2)
			vec.vy = -HEIGHT/2;
	}
	if (padd & PADLdown) {
		vec.vy += 8;
		if (vec.vy > HEIGHT/2)
			vec.vy = HEIGHT/2;
	}
	if (padd & PADLleft) {
		vec.vx -= 8;
		if (vec.vx < -WIDTH/2)
			vec.vx = -WIDTH/2;
	}
	if (padd & PADLright) {
		vec.vx += 8;
		if (vec.vx > WIDTH/2)
			vec.vx = WIDTH/2;
	}
	if (padd & PADk) 	ret = -1;

	RotMatrix(&ang, &m);
	TransMatrix(&m, &vec);

	SetRotMatrix(&m);
	SetTransMatrix(&m);

	return(ret);
}		

/*
 * initialize primitives
 */
static init_prim(db)
DB	*db;
{
	/* initialize bg */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* initialize wall */
	SetPolyFT4(&db->wall);
	db->wall.tpage = GetTPage(2, 0, VX, VY);
	setUVWH(&db->wall, 0, 0, VW*KW-1, (VH+1)*KH-1);
	SetSemiTrans(&db->wall, 0);
	setRGB0(&db->wall, 0xff, 0xff, 0xff);
}

/*
 * initialize kanji data texture
 */
initTexture(addr)
u_char *addr;
{
	RECT rect;
	u_short buf[BUFSIZE];
	int i, j;

	/* load kanji data */
	for (j = 0; j < KH; j++) {
		for (i = 0; i < KW; i++) {
			_get_font(buf, addr+(j*KW+i)*(KFONT*2));
        		/* send pixel data to VRAM */
        		rect.x = VX+i*VW;
        		rect.y = VY+j*(VH+1);
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
			*d++ = (((*p>>j)&0x01)==0x01)?COLOR:BLACK;
		for (j = 15; j >= 8; j--)
			*d++ = (((*p>>j)&0x01)==0x01)?COLOR:BLACK;
		p++;
	}
	for (i = 0; i < 16 ; i++) {
		*d++ = BLACK;
	}
}
