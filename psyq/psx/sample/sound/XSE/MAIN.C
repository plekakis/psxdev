/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	How to use Ut functions
 *
 *              Copyright (C) 1997  Sony Computer Entertainment
 *                                          All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h> 

/* defines for sound */
#define NOTICK				/* tickmode = SS_NOTICK */
#define VOICE_SET_MODE
#define VAB_HADDR       0x80010000L     /* vab header address */
#define VAB_BADDR       0x80015000L     /* vab data address */
#define PROGNO		1		/* program No. */
#define TONENO	9		/* tone No. (vibrato) */
/* #define TONENO		10	/* tone No. */
#define NOTE		60		/* note */
#define FINE		0		
#define VOL		64		/* volume */
#define VOICENUM	4		/* voice number */
#define PANSTART	0		/* start pan data */
#define PANEND		127		/* end pan data */
#define VOLSTART	0		/* start vol data */
#define VOLEND		127		/* end vol data */
#define AUTOTIME	64		/* delta time */

/* defines for graphics */
#define WIDTH		320		/* screen width */
#define HEIGHT		240		/* screen height */
#define CENTER		200		/* screen center */
#define OTSIZE		16		/* ot size */
#define FTSIZE		64		/* number of font spirtes */
#define BARNUM		3		/* number of bar */
#define TSIZE		5		/* tile size */
#define BSIZE		80		/* bar size / 2 */
#define YS		84
#define YE		94
#define YW		16
#define MAXBAR		CENTER+BSIZE	/* bar right */
#define MIDBAR		CENTER-TSIZE/2	/* bar center */
#define MINBAR		CENTER-BSIZE	/* bar left */
#define PLUS		4		/* tile move width */
#define BUFSIZE         7

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
unsigned char buf[BUFSIZE];

main()
{
	static  RECT bg = {0, 0, 640, 480};
	DB	*cdb;		/* current double buffer */
	int	i;		
	int 	dmy, flg;

	ResetCallback();
	ResetGraph(0);		/* reset graphic subsystem */
	SsInit();		/* reset sound */
	PadInit(0);		/* reset PAD */
	SetGraphDebug(0);

/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,      0, WIDTH, HEIGHT);
	SetDefDrawEnv(&db[1].draw, 0, HEIGHT, WIDTH, HEIGHT);
	SetDefDispEnv(&db[0].disp, 0, HEIGHT, WIDTH, HEIGHT);
	SetDefDispEnv(&db[1].disp, 0,      0, WIDTH, HEIGHT);

/* intialize font for screen print */
	FntLoad(640, 0);
	db[0].id = FntOpen(5, 55, MINBAR, 150, 0, FTSIZE);
	db[1].id = FntOpen(5, 55, MINBAR, 150, 0, FTSIZE);
	bzero(buf, BUFSIZE);

/* initialize primitive buffer */
	init_prim();
/* display */
	ClearImage(&bg, 0, 0, 0);
/* enable to display */
	SetDispMask(1);

/* set tick mode */
#ifdef NOTICK
	SsSetTickMode(SS_NOTICK);
#else /* NOTICK */
	SsSetTickMode(SS_TICK240);
#endif /* NOTICK */

/* open vab head */
	vab1 = SsVabOpenHead((unsigned char*)VAB_HADDR, -1);
	if (vab1 == -1) {
		printf("SsVabOpenHead: Open failed!\n");
		return;
	}
	if (SsVabTransBody((unsigned char*)VAB_BADDR, vab1) != vab1) {
		printf("SsVabTransBody: failed!\n");
		return;
	}
	SsVabTransCompleted(SS_WAIT_COMPLETED);

/* start sound system */
	SsStart();                   

	while (pad_read() == 0) {
		/* swap double buffer */
		cdb = (cdb==db) ? db+1 : db;	
		/* clear ordering table */
		ClearOTag(cdb->ot, OTSIZE);	
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
		VSync (0);			/* wait for V-BLNK */
		PutDispEnv(&cdb->disp);		/* update display environment */
		PutDrawEnv(&cdb->draw);		/* update drawing environment */
		DrawOTag(cdb->ot);	
	}
	PadStop();
	SsVabClose(vab1);	/* close vab data */
	SsEnd();		/* sound system end */
	SsQuit();               /* sound system exit */
	ResetGraph(3);		/* reset graphic subsystem */
        StopCallback();
	return(0);
}

/* 
 * initialize drawing primitives
 */
init_prim()
{
	int i, j;

	for (i = 0; i < BARNUM; i++) dx[i] = MIDBAR;
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

/* Read controll-pad */
int
pad_read()
{
	char *autopan = "autopan";
	char *autovol = "autovol";
	char *clear = "";
	static int key_on = 0;
	static int count_lu = 0;
	static int count_ld = 0;
	short pbend;
	short svol;
	short voll, volr;
	short spitch;
 	int ret = 0;

	u_long padd = PadRead(0);

        if (((padd & PADselect) > 0) && ((padd & PADstart) > 0)) {
		if (key_on) {
#ifdef VOICE_SET_MODE
			SsUtKeyOffV(1);
#else
			SsUtKeyOff(vc, vab1, PROGNO, TONENO, NOTE);
#endif /* VOICE_SET_MODE */
			ret = -1;
		}
	}
	
        if ((padd & PADstart) > 0) {
		if (!key_on) {
#ifdef VOICE_SET_MODE
			vc = SsUtKeyOnV(1, vab1, PROGNO, TONENO, 
					NOTE, FINE, VOL, VOL);
#else
			vc = SsUtKeyOn(vab1, PROGNO, TONENO, NOTE, FINE, 
				       VOL, VOL);
#endif /* VOICE_SET_MODE */
			if (vc != -1)
			    key_on = 1;
		}
        }
	
        if ((padd & PADselect) > 0) {
		if (key_on) {
#ifdef VOICE_SET_MODE
			SsUtKeyOffV(1);
			strcpy(buf, clear);
#else
			SsUtKeyOff(vc, vab1, PROGNO, TONENO, NOTE);
			strcpy(buf, clear);
#endif /* VOICE_SET_MODE */
			key_on = 0;
		}
		init_prim();
        }
	
	if ((padd & PADRdown) > 0) {
		SsUtAutoPan(vc, PANSTART, PANEND, AUTOTIME);
		strcpy(buf, autopan);
	}
	if ((padd & PADRup) > 0) {
		SsUtAutoVol(vc, VOLSTART, VOLEND, AUTOTIME);
		strcpy(buf, autovol);
	}
	
	if ((padd & PADL1) > 0) {
		if (key_on) {
			if ((dx[num] -= PLUS) < MINBAR)
			    dx[num] = MINBAR;
			switch(num) {
			      case 0:	
				pbend = (dx[num] - 32) / 2;
				SsUtPitchBend(vc, vab1, PROGNO, NOTE, pbend);
				break;
			      case 1:
				svol = (dx[num] - 32) / 2;
				SsUtSetVVol(vc, svol, svol); 
				break;
			      case 2:
				spitch = (dx[num] - 32) ;
				SsUtChangePitch(vc, vab1, PROGNO, NOTE,
						FINE, NOTE, spitch); 
				break;
			      default:
				break;
			}
		}
	}
	
	if ((padd & PADR1) > 0) {
		if (key_on) {
			if ((dx[num] += PLUS) > MAXBAR-TSIZE)
			    dx[num] = MAXBAR-TSIZE;
			switch(num) {
			      case 0:
				pbend = (dx[num] - 32) / 2;
				SsUtPitchBend(vc, vab1, PROGNO, NOTE, pbend);
				break;
			      case 1:
				svol = (dx[num] - 32) / 2;
				SsUtSetVVol(vc, svol, svol); 
				break;
			      case 2:
				spitch = (dx[num] - 32);
				SsUtChangePitch(vc, vab1, PROGNO, NOTE,
						FINE, NOTE, spitch); 
				break;
			      default:
				break;
			}
		}
	}
	
	if ((padd & PADLup) > 0) {
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
        if (((padd & PADselect) > 0) && ((padd & PADstart) > 0)) {
		ret = -1;
	}
	return(ret);
}

/* Print bar name */
print_name(int id)
{
	FntPrint(id, "status:%s\n", buf);
	FntPrint(id, "\n");
	FntPrint(id, "\n");
	FntPrint(id, "\n");
	FntPrint(id, "pitch bend\n");
	FntPrint(id, "\n");
	FntPrint(id, "change vol\n");
	FntPrint(id, "\n");
	FntPrint(id, "change pitch\n");
	FntFlush(id);
}
