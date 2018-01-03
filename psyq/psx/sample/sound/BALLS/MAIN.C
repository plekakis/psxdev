/*
 * balls with sound
 */
/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h>
#ifdef __psx__
#include <libsn.h>
#else  /* __psx__ */
#define pollhost()
#endif /* __psx__ */

/*
 * defines
 */
/* #define NOTICK				/* tickmode = SS_NOTICK */
#define VAB_HADDR       0x80020000L             /* vab header address */
#define VAB_BADDR       0x80025000L             /* vab data address */
#define SEQ_ADDR	0x80010000L		/* seq data address */
#define MVOL		127			/* main volume */
#define SVOL		127			/* seq data volume */
#define VOL		127			/* vab data volume */
#define START_X		10			/* ball start point */
#define START_Y		50			/* ball start point */
#define XWID		30			/* between ball width */

/*
 * Primitive Buffer
 */
#define OTSIZE		16			/* size of ordering table */
#define MAXOBJ		10			/* max sprite number */
typedef struct {		
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	SPRT_16		sprt[MAXOBJ];		/* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {		
	u_short x, y;			/* current point */
	u_short dx, dy;			/* verocity */
} POS;

/*
 * Limitations
 */
#define	FRAME_X		320		/* frame size (320x240) */
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* reflection point */
#define WALL_Y		(FRAME_Y-16)

void	init_prim(DB *);
void	init_point();
int	pad_read();

/*
 * Program No. and Pitch
 */
typedef struct {
	long prog;	/* program no */
	long pitch;	/* pitch */
} KON;

KON voice[MAXOBJ] = {
	{0x000f, 0x2900},
	{0x000f, 0x2a00},
	{0x000e, 0x2400},
	{0x000e, 0x4300},
	{0x000e, 0x3800},
	{0x000e, 0x3100},
	{0x000f, 0x2400},
	{0x000e, 0x3200},
	{0x000e, 0x3048},
	{0x000e, 0x2700}
}; 

POS	pos[MAXOBJ];	/* balls position */

short vab;	/* vab data id */
short seq;	/* seq data id */

char seq_table [SS_SEQ_TABSIZ * 1 * 1];  /* seq table size */

main()
{
	static	RECT	bg = {0, 0, 640, 480};
	
	DB	db[2];		/* double buffer */
	
	char	s[128];		/* strings to print */
	DB	*cdb;		/* current double buffer */
	u_long	*ot;		/* current OT */
	int	id;		/* font ID */
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, x, y;	/* work */
	int 	syflag[MAXOBJ];	/* sound on flag */

	ResetCallback();
	ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
	SsInit();		/* reset sound */
	SsSetTableSize (seq_table, 1, 1);

	PadInit(0);		/* reset PAD */
	SetGraphDebug(0);	/* set debug mode (0:off, 1:monitor, 2:dump) */

#ifdef NOTICK
	SsSetTickMode(SS_NOTICK);	/* set tick mode = NOTICK */
#else /* NOTICK */
	SsSetTickMode(SS_TICK240);	/* set tick mode = 1/240s */
#endif /* NOTICK */
	vab = SsVabOpenHead ((u_char *)VAB_HADDR, -1);
	if (vab < 0) {
	  printf ("SsVabOepnHead : failed!\n");
	  return;
        }
	if (SsVabTransBody ((u_char*)VAB_BADDR, vab) != vab) {
	  printf ("SsVabTransBody : failed!\n");
	  return;
	}
	SsVabTransCompleted (SS_WAIT_COMPLETED);
	seq = SsSeqOpen((u_long *)SEQ_ADDR, vab);	/* open seq data */

	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);

	init_prim(&db[0]);		/* initialize primitive buffers #0 */
	init_prim(&db[1]);		/* initialize primitive buffers #1 */
	init_point();			/* set initial geometries */

	/* display */
	ClearImage(&bg, 0, 0, 0);
	SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

	SsStart();			/* start sound */

	SsSetMVol(MVOL, MVOL);		/* set main volume */
	SsSeqSetVol(seq, SVOL, SVOL);	/* set seq data volume */
	SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);
					/* play seq data */
	/* initialize syflag */
	for (i = 0; i < MAXOBJ; i++)
		syflag[i] = 1;
	
	while (pad_read() == 0) {
		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
 		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
		/* update sprites */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		
		for (i = 0; i < MAXOBJ; i++, sp++, pp++) {

			x = pp->x;
			/* detect reflection */
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y) {
				y = WALL_Y*2 - y;
				if (syflag[i]) {
					SsVoKeyOff(voice[i].prog, 
						voice[i].pitch);
					SsVoKeyOn(voice[i].prog, 
						voice[i].pitch, VOL, VOL);
					syflag[i] = 0;
				}
			} else {
				if (!syflag[i]) {
					SsVoKeyOff(voice[i].prog, 
						voice[i].pitch);
					SsVoKeyOn(voice[i].prog, 
						voice[i].pitch, VOL, VOL);
					syflag[i] = 1;
				}
			}
			
			setXY0(sp, x, y);	/* update vertex */
			AddPrim(ot, sp);	/* apend to OT */
		}
		DrawSync(0);		/* wait for end of drawing */
#if 0
		pollhost ();
#endif
		VSync(0);		/* wait for V-BLNK */
#ifdef NOTICK
		SsSeqCalledTbyT();
#endif /* NOTICK */
		PutDispEnv(&cdb->disp); /* update display environment */
		PutDrawEnv(&cdb->draw); /* update drawing environment */
		DrawOTag(cdb->ot);
	}
	SsSeqStop(seq);		/* stop seq data */
	SsSeqClose(seq);	/* close seq data */
	SsVabClose(vab);	/* close vab data */
	SsEnd();		/* sound system end */
	SsQuit();

	PadStop();		/* pad stop */
	ResetGraph(3);		/* reset graphic subsystem (0:cold,1:warm) */
	StopCallback();
	/* exit(); /* */
	return 0;
}

/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

void init_prim(db)
DB	*db;
{
	u_short	clut[32];		/* CLUT entry */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */
	
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* load texture pattern and CLUT */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
	
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	}
	
	/* init sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);			/* set SPRT_16 primitve ID */
		SetSemiTrans(sp, 0);		/* semi-amibient is OFF */
		SetShadeTex(sp, 1);		/* shading&texture is OFF */
		setUV0(sp, 0, 0);		/* texture point is (0,0) */
		sp->clut = clut[i%32];		/* set CLUT */
	}
}	

/*
 * Initialize sprite position and verocity
 */
void init_point()
{
	POS	*p = pos;
	int	i;

	for (i = 0; i < MAXOBJ; i++, p++) {
		p->x  = i * XWID + START_X;
		p->y  = START_Y;
		p->dx = 0;
		p->dy = 0;
	}
}

/*
 * Read controll-pad
 */
int pad_read()
{
	static int key_on_lu = 0;
	static int key_on_ld = 0;
	static int key_on_ll = 0;
	static int key_on_lr = 0;
	static int key_on_l = 0;
	static int key_on_n = 0;
	static int key_on_ru = 0;
	static int key_on_rd = 0;
	static int key_on_rl = 0;
	static int key_on_rr = 0;
	int ret = 0;
/*	u_long	padd = PadRead(1); */
	u_long	padd;

	padd = PadRead(1);
	if(padd & PADLup) {
		if (!key_on_lu)	{
			pos[0].dy = 6;
			key_on_lu = 1;
		}
	} else {
		if (key_on_lu) {
			pos[0].dy = 0;
			key_on_lu = 0;
		}
	}
		
	if(padd & PADLdown) {
		if (!key_on_ld) {
			pos[1].dy = 6;
			key_on_ld = 1;
		}
	} else {
		if (key_on_ld) {
			pos[1].dy = 0;
			key_on_ld = 0;
		}
	}
	if(padd & PADLleft) { /* Old, PADj */
		if (!key_on_ll) {
			pos[2].dy = 6;
			key_on_ll = 1;
		}
	} else {
		if (key_on_ll) {
			pos[2].dy = 0;
			key_on_ll = 0;
		}
	}
	if(padd & PADLright)	{ /* Old, PADi */
		if (!key_on_lr) {
			pos[9].dy = 6;
			key_on_lr = 1;
		}
	} else {
		if (key_on_lr) {
			pos[9].dy = 0;
			key_on_lr = 0;
		}
	}
	if(padd & PADRleft) {
		if (!key_on_rl) {
			pos[3].dy = 6;
			key_on_rl = 1;
		}
	} else {
		if (key_on_rl) {
			pos[3].dy = 0;
			key_on_rl = 0;
		}
	}
	if(padd & PADRright) {
		if (!key_on_rr) {
			pos[4].dy = 6;
			key_on_rr = 1;
		}
	} else {
		if (key_on_rr) {
			pos[4].dy = 0;
			key_on_rr = 0;
		}
	}
	if(padd & PADl)	{
		if (!key_on_l) {
			pos[5].dy = 6;
			key_on_l = 1;
		}
	} else {
		if (key_on_l) {
			pos[5].dy = 0;
			key_on_l = 0;
		}
	}
	if(padd & PADn)	{
		if (!key_on_n) {
			pos[6].dy = 6;
			key_on_n = 1;
		}
	} else {
		if (key_on_n) {
			pos[6].dy = 0;
			key_on_n = 0;
		}
	}
	if(padd & PADRup) {
		if (!key_on_ru) {
			pos[7].dy = 6;
			key_on_ru = 1;
		}
	} else {
		if (key_on_ru) {
			pos[7].dy = 0;
			key_on_ru = 0;
		}
	}
	if(padd & PADRdown) {
		if (!key_on_rd) {
			pos[8].dy = 6;
			key_on_rd = 1;
		}
	} else {
		if (key_on_rd) {
			pos[8].dy = 0;
			key_on_rd = 0;
		}
	}
	
	if(padd & PADk)		ret = -1;

	return(ret);
}		
