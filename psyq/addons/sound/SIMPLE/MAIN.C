/*
 * sample..simple
 */
/*
 * $PSLibId: Runtime Library Release 3.6$
 */
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libsnd.h>

#define PARTLY /* SsVabTransBodyPartly() ‚ðŽg‚Á‚½•ªŠ„“]‘— */

extern void *memcpy ();
/*
 * defines
 */
/* #define NOTICK					/* tickmode = SS_NOTICK */
#define VAB_HADDR	0x80015000L		/* vab data address */
#define VAB_BADDR	0x80020000L		/* vab data address */
#define SEQ_ADDR	0x80010000L		/* seq data address */
#define MVOL		127			/* main volume */
#define SVOL		64			/* seq data volume */
#define OTSIZE		16			/* size of ordering table */
#define FTSIZE		64			/* size of font sprite */

typedef struct {
    DRAWENV		draw;	/* drawing environment */
    DISPENV		disp;	/* display environment */
    u_long		ot[OTSIZE]; /* ordering table */
    int		id;		/* font id */
} DB;

void	init_prim(DB *);
int	pad_read(DB *);

short vab;	/* vab data id */
short seq;	/* seq data id */

char seq_table[SS_SEQ_TABSIZ * 4 * 5]; /* seq data table */

#ifdef PARTLY
#define BUFSIZE 0x800
unsigned char buf [BUFSIZE];
volatile unsigned char *src = (unsigned char *) VAB_BADDR;
#endif /* PARTLY */

main()
{
    static RECT	bg = {0, 0, 640, 480};

    DB db[2];			/* double buffer */
    DB *cdb;			/* current double buffer */
    int	i;			/* work */
#ifdef PARTLY
    short vab_;			/* work */
    unsigned long top;
#endif /* PARTLY */

    ResetCallback();
    ResetGraph(0);		/* reset graphic subsystem */
    SsInit();			/* reset sound */
    SsSetTableSize (seq_table, 4, 5); /* keep seq data table area */
    PadInit(0);			/* reset PAD */
    FntLoad(640, 0);		/* load font */

    db[0].id = FntOpen(30, 30, 260, 180, 0, FTSIZE);
    db[1].id = FntOpen(30, 30, 260, 180, 0, FTSIZE);

    /* initialize environment for double buffer */
    SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
    SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
    SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);

    init_prim(&db[0]);		/* initialize primitive buffer */
    init_prim(&db[1]);		/* initialize primitive buffer */

    /* display */
    ClearImage(&bg, 0, 0, 0);
    SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

#ifdef NOTICK
    SsSetTickMode(SS_NOTICK);	/* set tick mode = NOTICK */
#else  /* NOTICK */
    SsSetTickMode(SS_TICK240);	/* set tick mode = TICK240 */
#endif /* NOTICK */

    vab = SsVabOpenHead ((u_char *)VAB_HADDR, -1);
    if (vab == -1) {
	printf ("SsVabOpenHead : failed !!!\n");
	return;
    }

#ifdef PARTLY
    top = 0;
    memcpy (buf, (unsigned char *)&src [top], BUFSIZE);
    while ((vab_ = SsVabTransBodyPartly (buf, BUFSIZE, vab)) != vab) {
	switch (vab_) {
	case -2:		/* Continue */
	    SsVabTransCompleted (SS_WAIT_COMPLETED);
	    top += BUFSIZE;
	    memcpy (buf, (unsigned char *) &src [top], BUFSIZE);
	    break;
	case -1:		/* Failed */
	    printf ("SsVabTransBodyPartly : failed !!!\n");
	    return;
	    break;
	default:
	    break;
	}
    }
#else /* PARTLY */
    if (SsVabTransBody ((unsigned char *)VAB_BADDR, vab) != vab) {
	printf ("SsVabTransBody : failed !!!\n");
	return;
    }
#endif /* PARTLY */
    SsVabTransCompleted (SS_WAIT_COMPLETED);

    seq = SsSeqOpen((u_long *)SEQ_ADDR, vab); /* open seq data */

    SsStart();			/* start sound */

    SsSetMVol(MVOL, MVOL);	/* set main volume */
    SsSeqSetVol(seq, SVOL, SVOL); /* set seq data volume */

    while (1) {
	cdb = (cdb == db) ? db+1 : db; /* swap double buffer ID */
	ClearOTag(cdb->ot, OTSIZE); /* clear ordering table */

	if (pad_read(cdb) != 0)
	    break;
		
	FntFlush(cdb->id);	/* print on screen */

	DrawSync(0);
	VSync(0);		/* wait for V-BLNK */
#ifdef NOTICK
	SsSeqCalledTbyT();
#endif /* NOTICK */
	PutDispEnv(&cdb->disp);	/* update display environment */
	PutDrawEnv(&cdb->draw);	/* update drawing environment */
	DrawOTag(cdb->ot);
    }
    SsSeqClose(seq);		/* close seq data */
    SsVabClose(vab);		/* close vab data */
    SsEnd();			/* sound system end */
    SsQuit();

    PadStop();			/* pad stop */
    ResetGraph(3);		/* reset graphic subsystem */
    StopCallback();
    return 0;
}

/*
 * Read controll-pad
 */
int pad_read(cdb)
     DB	*cdb;
{
    char *play   = "PLAY";
    char *stop   = "STOP";
    char *pause  = "PAUSE";
    char *setvol = "SETVOL";
    char *decr   = "DECRES";
    char *cres   = "CRES";
    char *rit    = "RITARD";
    char *acc    = "ACCELE";
    static char buf[6];
    static int key_on_lu = 0;
    static int key_on_ld = 0;
    static int key_on_j = 0;
    static int key_on_l = 0;
    static int key_on_n = 0;
    static int key_on_ru = 0;
    static int key_on_rd = 0;
    static int key_on_i = 0;
    static int seq_on = 0;
    static short vol = 64;
    static long tempo = 120;
    int ret = 0;
    u_long	padd = PadRead(1);

    if(padd & PADLup) {
	if (seq_on) {
	    /* volume up */
	    if (++vol > 127)
		vol = 127;
	    SsSeqSetVol(seq, vol, vol);
	    strcpy(buf, setvol);
	}
    }
    if(padd & PADLdown) {
	if (seq_on) {
	    /* volume down */
	    if (--vol < 0)
		vol = 0;
	    SsSeqSetVol(seq, vol, vol);
	    strcpy(buf, setvol);
	}
    }
    if(padd & PADLright) {
	if (seq_on) {
	    /* crescendo */
	    SsSeqSetCrescendo(seq, 127, 240);
	    strcpy(buf, cres);
	    vol = 127;
	}
    }
    if(padd & PADLleft) {
	if (seq_on) {
	    /* decrescendo */
	    SsSeqSetDecrescendo(seq, 127, 240);
	    strcpy(buf, decr);
	    vol = 0;
	}
    }
    if(padd & PADl)	{
	if (!key_on_l) {
	    if (seq_on) {
		/* accelerando */
		tempo += 30;
		if (tempo > 240)
		    tempo = 240;
		SsSeqSetAccelerando(seq, tempo, 240);
		strcpy(buf, acc);
	    }
	    key_on_l = 1;
	}
    } else {
	if (key_on_l) {
	    key_on_l = 0;
	}
    }
    if(padd & PADn)	{
	if (!key_on_n) {
	    if (seq_on) {
		/* ritardando */
		tempo -= 30;
		if (tempo < 30)
		    tempo = 30;
		SsSeqSetRitardando(seq, tempo, 240);
		strcpy(buf, rit);
	    }
	    key_on_n = 1;
	}
    } else {
	if (key_on_n) {
	    key_on_n = 0;
	}
    }
    if(padd & PADRup) {
	if (!key_on_ru) {
	    if (seq_on) {
		/* pause seq data */
		SsSeqPause(seq);
		strcpy(buf, pause);
	    }
	    key_on_ru = 1;
	}
    } else {
	if (key_on_ru) {
	    key_on_ru = 0;
	}
    }
    if(padd & PADRdown) {
	if (!key_on_rd) {
	    if (seq_on) {
		/* replay seq data */
		SsSeqReplay(seq);
		strcpy(buf, play);
	    }
	    key_on_rd = 1;
	}
    } else {
	if (key_on_rd) {
	    key_on_rd = 0;
	}
    }

    /* if(padd & PADk) */
    if((padd & PADk) && (padd & PADh))
	ret = -1;

    /* if(padd & PADi) { */
    if(padd & PADh)	{
	if (!key_on_i) {
	    if (!seq_on) {
		/* play seq data */
		SsSeqPlay(seq, SSPLAY_PLAY, SSPLAY_INFINITY);
		strcpy(buf, play);
		key_on_i = 1;
		seq_on = 1;
	    }
	}
    } else {
	if (key_on_i) {
	    key_on_i = 0;
	}
    }
    /* if(padd & PADj) { */
    if(padd & PADk) {
	if (!key_on_j) {
	    /* stop seq data */
	    SsSeqStop(seq);
	    strcpy(buf, stop);
	    key_on_j = 1;
	    seq_on = 0;
	    vol = 64;
	    tempo = 120;
	}
    } else {
	if (key_on_j) {
	    key_on_j = 0;
	}
    }
	
    if (!seq_on)
	strcpy(buf, stop);

    FntPrint(cdb->id, "volume : %d\n", vol); /* print volume */
    FntPrint(cdb->id, "tempo  : %d\n", tempo); /* print tempo */
    FntPrint(cdb->id, "\n");	/* print \n */
    FntPrint(cdb->id, "status : %s\n", buf); /* print status */

    return(ret);
}		

void init_prim(db)
     DB	*db;
{
    /* initialize double buffer */
    db->draw.isbg = 1;
    setRGB0(&db->draw, 60, 120, 120);
}
