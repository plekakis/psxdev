/*
 * sound effect
 */
/*
 * $PSLibId: Runtime Library Release 3.6$
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h> 
#ifdef __psx__
#include <libsn.h> 
#else
#define pollhost()
#endif

/*
 * defines for sound
 */
#define NOTICK				/* tickmode = SS_NOTICK */
#define VAB_HADDR       0x80010000L     /* vab header address */
#define VAB_BADDR       0x80015000L     /* vab data address */
#define PROGNO		1		/* program No. */
#define TONENO		10		/* tone No. */
/* #define TONENO	9		/* tone No. (vibrato) */
#define NOTE		60		/* note */
#define FINE		0		
#define VOL		127		/* volume */
#define PANT		1		/* pan change tick */
#define VOLT		1		/* volume change tick */

/*
 * defines for graphics
 */
#define WIDTH		320		/* screen width */
#define HEIGHT		240		/* screen height */
#define CENTER		WIDTH/2		/* screen center */
#define OTSIZE		16		/* ot size */
#define FTSIZE		64		/* number of font spirtes */
#define BARNUM		3		/* number of bar */
#define TSIZE		10		/* tile size */
#define BSIZE		128		/* bar size / 2 */
#define YS		50
#define YE		70
#define YW		40
#define MAXBAR		CENTER+BSIZE	/* bar right */
#define MIDBAR		CENTER-TSIZE/2	/* bar center */
#define MINBAR		CENTER-BSIZE	/* bar left */
#define PLUS		4		/* tile move width */

typedef struct {
	DRAWENV		draw;		/* drawing environment */
	DISPENV		disp;		/* display environment */
	int 		id;		/* font id */
	u_long		ot[OTSIZE];	/* ordering table */
	POLY_F4		bar[BARNUM];	/* bar polygon */
	POLY_F4		tile[BARNUM];	/* moving tile polygon */
	POLY_F4		center[BARNUM];	/* center tile polygon */
} DB;

DB db[2];	/* double buffer */

short vab1;	/* vab id */
short vc;	/* voice id */

int dx[BARNUM];	/* tile buffer */
int num;	/* tile number */

main()
{
	static RECT	bg = {0, 0, 640, 480};

	DB	*cdb;		/* current double buffer */
	int	i;		/* work */
	int 	dmy, flg;	/* work */

	ResetCallback();
	ResetGraph(0);		/* reset graphic subsystem */
	SsInit ();		/* reset sound */
	PadInit(0);		/* reset PAD */
	SetGraphDebug (0);

/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,      0, WIDTH, HEIGHT);
	SetDefDrawEnv(&db[1].draw, 0, HEIGHT, WIDTH, HEIGHT);
	SetDefDispEnv(&db[0].disp, 0, HEIGHT, WIDTH, HEIGHT);
	SetDefDispEnv(&db[1].disp, 0,      0, WIDTH, HEIGHT);

/* intialize font for screen print */
	FntLoad(640, 0);
	db[0].id = FntOpen(5, 55, MINBAR, 150, 0, FTSIZE);
	db[1].id = FntOpen(5, 55, MINBAR, 150, 0, FTSIZE);

/* initialize primitive buffer */
	init_prim();

/* display */
	ClearImage(&bg, 0, 0, 0);
	SetDispMask(1);		/* enable to display */

	/* set tick mode */
#ifdef NOTICK
	SsSetTickMode (SS_NOTICK);
#else /* NOTICK */
	SsSetTickMode (SS_TICK240);
#endif /* NOTICK */

	/* open vab head */
	vab1 = SsVabOpenHead((unsigned char*)VAB_HADDR, -1);
	if (vab1 == -1) {
		printf ("SsVabOpenHead: Open failed!\n");
		return;
	}

	if (SsVabTransBody ((unsigned char*)VAB_BADDR, vab1) != vab1) {
		printf ("SsVabTransBody: failed!\n");
		return;
	}
	SsVabTransCompleted (SS_WAIT_COMPLETED);

	SsStart ();                   /* start sound system */

	while (pad_read() == 0) {
		cdb = (cdb==db) ? db+1 : db;	/* swap double buffer */
		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */

		/* set (x, y) */
		setXY4(&cdb->tile[num], dx[num], YS+num*YW, dx[num]+TSIZE, 
		       YS+num*YW, dx[num], YE+num*YW, dx[num]+TSIZE, YE+num*YW);

		/* append to ordering table */
		for (i = 0; i < BARNUM; i++) {
			AddPrim(cdb->ot, &cdb->bar[i]);
			AddPrim(cdb->ot+1, &cdb->center[i]);
			AddPrim(cdb->ot+2, &cdb->tile[i]);
		}

		print_name(cdb->id);		/* print bar name */

		DrawSync(0);			/* wait for end of drawing */
#ifdef NOTICK
		SsSeqCalledTbyT();
#endif /* NOTICK */
#if 0
		pollhost ();
#endif
		VSync (0);			/* wait for V-BLNK */
		PutDispEnv(&cdb->disp);		/* update display environment */
		PutDrawEnv(&cdb->draw);		/* update drawing environment */
		DrawOTag(cdb->ot);	
	}
	PadStop();		/* pad stop */
	SsVabClose(vab1);	/* close vab data */
	SsEnd();		/* sound system end */
	SsQuit();
	ResetGraph(3);		/* reset graphic subsystem */
        StopCallback();
	return 0;
}

/* 
 * initialize drawing primitives
 */
init_prim()
{
	int i, j;	/* work */

	for (i = 0; i < BARNUM; i++)
		dx[i] = MIDBAR;
	num = 0;

	for (j = 0; j < 2; j++) {
		db[j].draw.isbg = 1;
		setRGB0(&(db[j].draw), 0x0, 0xff, 0x0);

		for (i = 0; i < BARNUM; i++) {
			/* intialize bar */
			SetPolyF4(&db[j].bar[i]);
			setRGB0(&db[j].bar[i], 0x0, 0x0, 0xff);
			setXY4(&db[j].bar[i], MINBAR, YS+i*YW, MAXBAR, YS+i*YW,
				 MINBAR, YE+i*YW, MAXBAR, YE+i*YW);

			/* intialize moving tile */
			SetPolyF4(&db[j].tile[i]);
			setRGB0(&db[j].tile[i], 0xff, 0xff, 0xff);
			setXY4(&db[j].tile[i], dx[i], YS+i*YW, dx[i]+TSIZE, 
				YS+i*YW, dx[i], YE+i*YW, dx[i]+TSIZE, YE+i*YW);

			/* intialize center tile */
			SetPolyF4(&db[j].center[i]);
			setRGB0(&db[j].center[i], 0x0, 0xff, 0xff);
			setXY4(&db[j].center[i], dx[i], YS+i*YW, dx[i]+TSIZE, 
				YS+i*YW, dx[i], YE+i*YW, dx[i]+TSIZE, YE+i*YW);
		}
		setRGB0(&db[j].tile[0], 0xff, 0x0, 0x0);
	}
}

/*
 * Read controll-pad
 */
int
pad_read()
{
	static int key_on = 0;
	static int key_on_v = 0;
	static int key_on_p = 0;
	static int count_lu = 0;
	static int count_ld = 0;
	static short pans = 0x40;
	static short vols = 0x40;
	short pbend;
	short pane;
	short vole;
	u_long padd = PadRead(0);
	int ret = 0;
	int i;

        if (((padd & PADk) > 0) &&
	    ((padd & PADh) > 0)) {
		/* quit */
		if (key_on)
			/* Voice key off */
			SsUtKeyOff(vc, vab1, PROGNO, TONENO, NOTE);
		ret = -1;
        }

        if ((padd & PADh) > 0) {
		if (!key_on) {
			/* Voice Key On */
			vc = SsUtKeyOn(vab1, PROGNO, TONENO, NOTE, FINE, 
					VOL, VOL);
			if (vc != -1)
				key_on = 1;
		}
        }

        if ((padd & PADk) > 0) {
		if (key_on) {
			/* Voice Key Off */
			SsUtKeyOff(vc, vab1, PROGNO, TONENO, NOTE);
			pans = 0x40;
			vols = 0x40;
			key_on = 0;
			key_on_v = 0;
		}
		init_prim();
        }

	if ((padd & PADn) > 0) {
		if (key_on) {
			if ((dx[num] -= PLUS) < MINBAR)
				dx[num] = MINBAR;
			switch(num) {
			case 0:	/* set auto pan */
				pane = 127 - ((dx[num] - 32) / 2);
				SsUtAutoPan(vc, pans, pane, PANT);
				pans = pane;
				break;

			case 1:	/* set auto volume */
				vole = (dx[num] - 32) / 2;
				SsUtAutoVol(vc, vols, vole, VOLT);
				vols = vole;
				break;

			case 2:	/* set pitch bend */
				pbend = (dx[num] - 32) / 2;
				SsUtPitchBend(vc, vab1, PROGNO, NOTE, pbend);
				break;

			default:
				break;
			}
		}
	}

	if ((padd & PADl) > 0) {
		if (key_on) {
			if ((dx[num] += PLUS) > MAXBAR-TSIZE)
				dx[num] = MAXBAR-TSIZE;
			switch(num) {
			case 0:	/* set pan */
				pane = 127 - ((dx[num] - 32) / 2);
				SsUtAutoPan(vc, pans, pane, PANT);
				pans = pane;
				break;

			case 1:	/* set volume */
				vole = (dx[num] - 32) / 2;
				SsUtAutoVol(vc, vols, vole, VOLT);
				vols = vole;
				break;

			case 2:	/* set pitch bend */
				pbend = (dx[num] - 32) / 2;
				SsUtPitchBend(vc, vab1, PROGNO, NOTE, pbend);
				break;

			default:
				break;
			}
		}
	}

	if ((padd & PADLup) > 0) {
		/* tile up */
		if (++count_lu > 10) {
			if (--num < 0)
				num = 0;
			setRGB0(&db[0].tile[num+1], 0xff, 0xff, 0xff);
			setRGB0(&db[1].tile[num+1], 0xff, 0xff, 0xff);
			setRGB0(&db[0].tile[num], 0xff, 0x0, 0x0);
			setRGB0(&db[1].tile[num], 0xff, 0x0, 0x0);
			count_lu = 0;
		}
	}

	if ((padd & PADLdown) > 0) {
		/* tile down */
		if (++count_ld > 10) {
			if (++num > BARNUM-1)
				num = BARNUM-1;
			setRGB0(&db[0].tile[num-1], 0xff, 0xff, 0xff);
			setRGB0(&db[1].tile[num-1], 0xff, 0xff, 0xff);
			setRGB0(&db[0].tile[num], 0xff, 0x0, 0x0);
			setRGB0(&db[1].tile[num], 0xff, 0x0, 0x0);
			count_ld = 0;
		}
	}

	return(ret);
}

/*
 * Print bar name 
 */
print_name(int id)
{
	FntPrint(id, "PAN\n");
	FntPrint(id, "\n\n\n\n");
	FntPrint(id, "VOL\n");
	FntPrint(id, "\n\n\n\n");
	FntPrint(id, "PIT\n");
	FntFlush(id);		/* print on screen */
}
