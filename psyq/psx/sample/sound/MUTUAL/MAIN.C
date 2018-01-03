/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 * -*- c -*-
 * $RCSfile: main.c,v $
 *
 * Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *				     All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/
/*
 *		    balls: sample program
 *
 *		Copyright (C) 1993, 1994
 *		by Sony Corporation,
 *		   Sony Computer Entertainment Inc.
 *		All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Aug,31,1993	suzu
 *	2.00		Nov,17,1993	suzu	(using 'libgpu)
 *	3.00		Dec.27.1993	suzu	(rewrite)
 *	3.01		Dec.27.1993	suzu	(for newpad)
 *	4.00		Sep.28.1994	kaol	(for CD, sound)
 */

#ifndef lint
static char rcsid [] = "$Id: main.c,v 1.13 1997/09/17 08:22:13 kaol Exp noda $ : \
	Copyright (C) by 1993, 1994 Sony Computer Entertainment Inc.";
#endif
#define SOUND /**/
#include <sys/types.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h>
#include <libspu.h>
#include <libcd.h>
#ifdef SOUND
#ifdef __psx__
#include <libsn.h>
#else
#define pollhost()
#endif
#endif

#ifdef DEBUG
#define PRINTF(x) printf x
#else
#define PRINTF(x)
#endif

/*
 * Kanji Printf
 */
/* #define KANJI /**/

/* ----------------------------------------------------------
 *
 *	Compile Options
 *
 *	WITH_CDROM:	When #defined, file is read
 *			from the CD. Without a #define, it is  
 *			read from main memory. (VH, VB, and SEQ 
 *			are transferred to main memory ahead of 
 *			time by pqbload).
 *			
 *
 *	CD_NOWAIT:	When #defined, waits for reaction from 
 *			CD while checking in the VSync loop. 
 *			Exits as is if the transfer is not
 *			finished.
 *
 *	CD_SCAN_EVERYTIME:
 *			When #defined, a CdSearchFile is carried 
 *			out each time a file is read-in from the 
 *			CD.Without a #define, it is read in ahead 
 *			of time before the VSync loop and that 
 *			information is retained.
 *
 *	START_PLAY:	When #defined, sound data is first read 
 *			in from main memory and performance 
 *			begins without direction from the pad. 
 *			(It is necessary to transfer VH, VB, SEQ 
 *			to main memory ahead of time using 
 *			pqbload).Without a #define, start up 
 *			first occurs without sound being 
 *			produced.
 *
 *	Pad Assignment
 *
 *		L1 Decrease balls	R1 Increase balls
 *       
 *	        ^ load sound 1            O load sound 2
 *	      <   >  play sound 1  	O   O play sound 2
 *	      ^		      		^
 *	      stop sound 1	      	stop sound 2 */

#define WITH_CDROM /**/
#define CD_NOWAIT /**/
/* #define CD_SCAN_EVERYTIME /**/
/* #define START_PLAY /**/

#ifdef SOUND
/*
 * Primitive Buffer
 */
#define VAB1_HADDR	0x80020000L		/* vab header address */
#define VAB1_BADDR	0x80025000L		/* vab data address */
#define VAB2_HADDR	0x80060000L		/* vab header address */
#define VAB2_BADDR	0x80065000L		/* vab data address */
#define SEQ1_ADDR	0x80010000L		/* seq data address */
#define SEQ2_ADDR	0x80018000L		/* seq data address */
#define MVOL		127			/* main volume */
#define SVOL		127			/* seq data volume */

#define VH1_FILE "\\PSX\\DATA\\VAB\\MUTUAL.VH;1"
#define VH2_FILE "\\PSX\\DATA\\VAB\\MUTUAL.VH;1"

#define VB1_FILE "\\PSX\\DATA\\VAB\\MUTUAL.VB;1"
#define VB2_FILE "\\PSX\\DATA\\VAB\\MUTUAL.VB;1"

#define SEQ1_FILE "\\PSX\\DATA\\SEQ\\MUTUAL1.SEQ;1"
#define SEQ2_FILE "\\PSX\\DATA\\SEQ\\MUTUAL2.SEQ;1"

#endif /* SOUND */

#define OTSIZE		16	/* size of ordering table */
#define MAXOBJ		3000	/* max sprite number */
typedef struct {		
    DRAWENV		draw;	/* drawing environment */
    DISPENV		disp;	/* display environment */
    u_long		ot[OTSIZE]; /* ordering table */
    SPRT_16		sprt[MAXOBJ]; /* 16x16 fixed-size sprite */
} DB;

/*
 * Position Buffer
 */
typedef struct {		
    u_short x, y;		/* current point */
    u_short dx, dy;		/* verocity */
} POS;

/*
 * Limitations
 */
#define	FRAME_X		320	/* frame size (320x240) */
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* reflection point */
#define WALL_Y		(FRAME_Y-16)

#ifdef SOUND
short vab1, vab2;		/* vab data id */
short seq1, seq2;		/* seq data id */

long read_flag;		/* Data read flag */
long which_read;
long vtsize;

char seq_table [SS_SEQ_TABSIZ * 4 * 5]; /* seq data table */

#define SECTOR_SIZ 2048
#define SEC_BUF 8
#define BUFSIZE (SECTOR_SIZ*SEC_BUF)
unsigned char buf [BUFSIZE];
void addLoc (CdlLOC *d, CdlLOC *s);
long readSoundDataFromCD (void);
void getCDlocation (void);

CdlFILE fdvb1,  fdvb2;		/* for CD access */
CdlFILE fdvh1,  fdvh2;		/* for CD access */
CdlFILE fdseq1, fdseq2;		/* for CD access */

CdlFILE fdvb1_,  fdvb2_;	/* for CD access */
CdlFILE fdvh1_,  fdvh2_;	/* for CD access */
CdlFILE fdseq1_, fdseq2_;	/* for CD access */

CdlLOC addsize;			/* for CD's next data */
#endif /* SOUND */

main()
{
    POS	pos[MAXOBJ];
    DB	db[2];			/* double buffer */
	
    char	s[128];		/* strings to print */
    DB	*cdb;			/* current double buffer */
    int	nobj = 1;		/* object number */
    u_long	*ot;		/* current OT */
    SPRT_16	*sp;		/* work */
    POS	*pp;			/* work */
    int	i, cnt, x, y;		/* work */

#ifdef SOUND
    long fflag;			/* 0 if SsVabTransBodyPartly is finished */
#endif

    /* ----------------------------------------------------------------
     *		Initialize interrupt environment. */

    ResetCallback();

    CdInit ();

#ifdef SOUND
    SsInit ();			/* reset sound */
    SsSetTableSize (seq_table, 4, 5); /* keep seq data table area */
#endif

    PadInit(0);			/* reset PAD */
    ResetGraph(0);		/* reset graphic subsystem (0:cold,1:warm) */
    SetGraphDebug(0);		/* set debug mode (0:off, 1:monitor, 2:dump) */

    /* initialize environment for double buffer */
    SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
    SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
    SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
    SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);
	
    /* init font environment */
#ifdef KANJI
    KanjiFntOpen(16, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#else  /* KANJI */
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));
#endif /* KANJI */

    init_prim(&db[0]);		/* initialize primitive buffers #0 */
    init_prim(&db[1]);		/* initialize primitive buffers #1 */
    init_point(pos);		/* set initial geometries */

    /* display */
    SetDispMask(1);		/* enable to display (0:inhibit, 1:enable) */

#ifdef SOUND
#ifdef WITH_CDROM
    getCDlocation ();
#endif /* WITH_CDROM */

    SsSetTickMode (SS_TICK60);	/* set tick mode = 1/480 */

#ifdef START_PLAY
    /* ----------------------------------------------------------------
     * Transition of VAB
     * ---------------------------------------------------------------- */

    vab1 = SsVabOpenHead ((u_char *)VAB1_HADDR, -1);
    if (vab1 < 0) {
	printf ("SsVabOpenHead [1]: Can NOT open header.\n");
	return;
    }
    if (SsVabTransBody ((unsigned char *) VAB1_BADDR, vab1) != vab1) {
	printf ("SsVabTransBody [1]: failed !!!\n");
	return;
    }
    SsVabTransCompleted (SS_WAIT_COMPLETED);

    seq1 = SsSeqOpen ((u_long *)SEQ1_ADDR, vab1); /* open seq data */
    SsSeqSetVol (seq1, SVOL, SVOL); /* set seq data volume */
#endif /* START_PLAY */

    SsSetMVol (MVOL, MVOL);	/* set main volume */

    SsStart2 ();		/* start sound */

#ifdef START_PLAY
    SsSeqPlay (seq1, SSPLAY_PLAY, SSPLAY_INFINITY);
#endif /* START_PLAY */

    read_flag = 0;
    which_read = 0;
    vtsize = 0;
#ifndef START_PLAY
    vab1 = -1;
    seq1 = -1;
#endif /* START_PLAY */
    vab2 = -1;
    seq2 = -1;
#endif /* SOUND */

    while ((nobj = pad_read(nobj)) > 0) {
	cdb  = (cdb==db)? db+1: db; /* swap double buffer ID */

	/* dump DB environment */
	/*DumpDrawEnv(&cdb->draw);*/
	/*DumpDispEnv(&cdb->disp); */
	/*DumpTPage(cdb->draw.tpage);*/
		
	/* clear ordering table */
	ClearOTag(cdb->ot, OTSIZE);	
		
	/* update sprites */
	ot = cdb->ot;
	sp = cdb->sprt;
	pp = pos;
		
	for (i = 0; i < nobj; i++, sp++, pp++) {
	    /* detect reflection */
	    if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
		x = WALL_X*2 - x;
	    if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
		y = WALL_Y*2 - y;
			
	    setXY0(sp, x, y);	/* update vertex */
	    AddPrim(ot, sp);	/* apend to OT */
	}
	DrawSync(0);		/* wait for end of drawing */

#ifdef SOUND
	if (read_flag != 0) {
	    readSoundDataFromCD (); /**/
	}
#endif

	cnt = VSync(1);		/* check for count */
	VSync(0);		/* wait for V-BLNK */
	PutDispEnv(&cdb->disp); /* update display environment */
	PutDrawEnv(&cdb->draw); /* update drawing environment */
	DrawOTag(cdb->ot);
	/*DumpOTag(cdb->ot);*/
#ifdef KANJI
	KanjiFntPrint("‹Ê‚Ì”%d\n", nobj);
	KanjiFntPrint("ŽžŠÔ=%d\n", cnt);
	KanjiFntFlush(-1);
#else  /* KANJI */
	FntPrint("sprite = %d\n", nobj);
	FntPrint("total time = %d\n", cnt);
	FntFlush(-1);
#endif /* KANJI */
    }

    /* ----------------------------------------------------------------
     *		Finalize this sample */

#ifdef SOUND
    SsSetMVol (0, 0);		/* set main volume */

    if (seq1 != -1)
	SsSeqClose (seq1);	/* close seq data */
    if (seq2 != -1)
	SsSeqClose (seq2);	/* close seq data */
    if (vab1 != -1)
	SsVabClose (vab1);	/* close vab data */
    if (vab2 != -1)
	SsVabClose (vab2);	/* close vab data */
	
    SsEnd ();			/* sound system end */
#endif /* SOUND */

    PadStop();
    ResetGraph (3);		/* reset graphic subsystem */
    StopCallback ();		/* stop callback processing */

    return;
}

/*
 * Initialize drawing Primitives
 */
#include "balltex.h"

init_prim(db)
     DB	*db;
{
    u_short	clut[32];	/* CLUT entry */
    SPRT_16	*sp;		/* work */
    int	i;			/* work */
	
    /* inititalize double buffer */
    db->draw.isbg = 1;
    setRGB0(&db->draw, 60, 120, 120);
	
    /* load texture pattern and CLUT */
    db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifndef KANJI
    DumpTPage(db->draw.tpage);
#endif /* !KANJI */
	
    for (i = 0; i < 32; i++) {
	clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	/*DumpClut(clut[i]);*/
    }
	
    /* init sprite */
    for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
	SetSprt16(sp);		/* set SPRT_16 primitve ID */
	SetSemiTrans(sp, 0);	/* semi-amibient is OFF */
	SetShadeTex(sp, 1);	/* shading&texture is OFF */
	setUV0(sp, 0, 0);	/* texture point is (0,0) */
	sp->clut = clut[i%32];	/* set CLUT */
    }
}	

/*
 * Initialize sprite position and verocity
 */
init_point(pos)
     POS	*pos;
{
    int	i;
    for (i = 0; i < MAXOBJ; i++) {
	pos->x  = rand();
	pos->y  = rand();
	pos->dx = (rand() % 4) + 1;
	pos->dy = (rand() % 4) + 1;
	pos++;
    }
}

/*
 * Read controll-pad
 */
pad_read(n)
     int	n;		/* current object number */		
{
    u_long	padd = PadRead(1);

#ifdef SOUND
    static long _1start = 0;
    static long _1stop = 0;
    static long _r1 = 0;
    static long _r2 = 0;
    static long _2start = 0;
    static long _2stop = 0;
#ifdef START_PLAY
    static long seq1start = 1;
#else /* START_PLAY */
    static long seq1start = 0;
#endif /* START_PLAY */
    static long seq2start = 0;
#endif /* SOUND */

    if(padd & PADl)	n += 4;	/* left '+' key up */
    if(padd & PADn)	n -= 4;	/* left '+' key down */
	
    if (padd & PADk)
	return 0;

#ifdef SOUND
    /* VAB1 read */
    if (padd & PADLup)	{
	if (_r1 == 0) {
	    _r1 = 1;
	    if (read_flag == 0) {
		if (vab1 != -1) {
		    SsVabClose (vab1);
		    SsSeqClose (seq1);
		    vab1 = -1;
#ifdef WITH_CDROM
#ifndef CD_SCAN_EVERYTIME
		    fdvh1.pos.minute  = fdvh1_.pos.minute;
		    fdvh1.pos.second  = fdvh1_.pos.second;
		    fdvh1.pos.sector  = fdvh1_.pos.sector;
		    fdvb1.pos.minute  = fdvb1_.pos.minute;
		    fdvb1.pos.second  = fdvb1_.pos.second;
		    fdvb1.pos.sector  = fdvb1_.pos.sector;
		    fdseq1.pos.minute = fdseq1_.pos.minute;
		    fdseq1.pos.second = fdseq1_.pos.second;
		    fdseq1.pos.sector = fdseq1_.pos.sector;
#endif /* CD_SCAN_EVERYTIME */
#endif /* WITH_CDROM */
		}
		which_read = 1;
		read_flag ++;
	    }
	}
    } else {
	if (_r1 != 0) {
	    _r1 = 0;
	}
    }

    /* VAB2 read */
    if (padd & PADRup)	{
	if (_r2 == 0) {
	    _r2 = 1;
	    if (read_flag == 0) {
		if (vab2 != -1) {
		    SsVabClose (vab2);
		    SsSeqClose (seq2);
		    vab2 = -1;
#ifdef WITH_CDROM
#ifndef CD_SCAN_EVERYTIME
		    fdvh2.pos.minute  = fdvh2_.pos.minute;
		    fdvh2.pos.second  = fdvh2_.pos.second;
		    fdvh2.pos.sector  = fdvh2_.pos.sector;
		    fdvb2.pos.minute  = fdvb2_.pos.minute;
		    fdvb2.pos.second  = fdvb2_.pos.second;
		    fdvb2.pos.sector  = fdvb2_.pos.sector;
		    fdseq2.pos.minute = fdseq2_.pos.minute;
		    fdseq2.pos.second = fdseq2_.pos.second;
		    fdseq2.pos.sector = fdseq2_.pos.sector;
#endif /* CD_SCAN_EVERYTIME */
#endif /* WITH_CDROM */
		}
		which_read = 2;
		read_flag ++;
	    }
	}
    } else {
	if (_r2 != 0) {
	    _r2 = 0;
	}
    }

    /* SEQ1 start */
    if (padd & PADLright) {
	if (_1start == 0) {
	    _1start = 1;
	    if (seq1start == 0) {
		/* play seq data */
		if (seq1 != -1) {
		    SsSeqPlay (seq1, SSPLAY_PLAY, SSPLAY_INFINITY);
		    seq1start = 1;
		}
	    }
	}
    } else {
	if (_1start != 0) {
	    _1start = 0;
	}
    }

    /* SEQ1 stop */
    if (padd & PADLleft) {
	if (_1stop == 0) {
	    _1stop = 1;
	    if (seq1start == 1) {
		/* stop seq data */
		SsSeqStop (seq1);
		seq1start = 0;
	    }
	}
    } else {
	if (_1stop != 0) {
	    _1stop = 0;
	}
    }

    /* SEQ2 start */
    if (padd & PADRright) {
	if (_2start == 0) {
	    _2start = 1;
	    if (seq2start == 0) {
		/* play seq data */
		if (seq2 != -1) {
		    SsSeqPlay (seq2, SSPLAY_PLAY, SSPLAY_INFINITY);
		    seq2start = 1;
		}
	    }
	}
    } else {
	if (_2start != 0) {
	    _2start = 0;
	}
    }

    /* SEQ2 stop */
    if (padd & PADRleft) {
	if (_2stop == 0) {
	    _2stop = 1;
	    if (seq2start == 1) {
		/* stop seq data */
		SsSeqStop (seq2);
		seq2start = 0;
	    }
	}
    } else {
	if (_2stop != 0) {
	    _2stop = 0;
	}
    }
#endif /* SOUND */
    limitRange(n, 1, MAXOBJ-1);	/* see libgpu.h */
    return(n);
}		

#ifdef SOUND
void
getCDlocation (void)
{
    long size, size1, size2;

    addsize.minute = 0;
    addsize.second = 0;
    /* data expression is BCD */
    size = BUFSIZE / SECTOR_SIZ;
    if (size > 10) {
	size1 = size % 10;
	size2 = size / 10;
	size  = (size2 << 4) | size1;
    }
    addsize.sector = size;

#ifndef CD_SCAN_EVERYTIME
    if (CdSearchFile (&fdvh1, VH1_FILE) == 0) {
	printf ("file not find : %s\n", VH1_FILE);
	return;
    }
    fdvh1_.pos.minute = fdvh1.pos.minute;
    fdvh1_.pos.second = fdvh1.pos.second;
    fdvh1_.pos.sector = fdvh1.pos.sector;

    if (CdSearchFile (&fdvb1, VB1_FILE) == 0) {
	printf ("file not find : %s\n", VB1_FILE);
	return;
    }
    fdvb1_.pos.minute = fdvb1.pos.minute;
    fdvb1_.pos.second = fdvb1.pos.second;
    fdvb1_.pos.sector = fdvb1.pos.sector;

    if (CdSearchFile (&fdvh2, VH2_FILE) == 0) {
	printf ("file not find : %s\n", VH2_FILE);
	return;
    }
    fdvh2_.pos.minute = fdvh2.pos.minute;
    fdvh2_.pos.second = fdvh2.pos.second;
    fdvh2_.pos.sector = fdvh2.pos.sector;

    if (CdSearchFile (&fdvb2, VB2_FILE) == 0) {
	printf ("file not find : %s\n", VB2_FILE);
	return;
    }
    fdvb2_.pos.minute = fdvb2.pos.minute;
    fdvb2_.pos.second = fdvb2.pos.second;
    fdvb2_.pos.sector = fdvb2.pos.sector;

    if (CdSearchFile (&fdseq1, SEQ1_FILE) == 0) {
	printf ("file not find : %s\n", SEQ1_FILE);
	return;
    }
    fdseq1_.pos.minute = fdseq1.pos.minute;
    fdseq1_.pos.second = fdseq1.pos.second;
    fdseq1_.pos.sector = fdseq1.pos.sector;

    if (CdSearchFile (&fdseq2, SEQ2_FILE) == 0) {
	printf ("file not find : %s\n", SEQ2_FILE);
	return;
    }
    fdseq2_.pos.minute = fdseq2.pos.minute;
    fdseq2_.pos.second = fdseq2.pos.second;
    fdseq2_.pos.sector = fdseq2.pos.sector;
#endif /* CD_SCAN_EVERYTIME */
}

unsigned char result [8];

long
readSoundDataFromCD (void)
{
    static short vab, vab_;
    static short seq;
    static long vb_seek1;
    long addr;
    long size, size1, size2;

    /* ================================================================
     *  VAB/SEQ read-in during drawing/performance and 
     *  VAB transfer */
    
    switch (read_flag) {

    case 0:
	/* Normal Loop */
	break;

    case 1:
	/* ----------------------------------------------------------------
	 * VAB read in  */

	/*
	 *	VH read in */

#ifdef WITH_CDROM
	/* Head position indication */
	FntPrint("VH: CD set locate (%d)\n", which_read);
	switch (which_read) {
	case 1:
#ifdef CD_SCAN_EVERYTIME
	    if (CdSearchFile (&fdvh1, VH1_FILE) == 0) {
		printf ("file not find : %s\n", VH1_FILE);
		return -1;
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdvh1.pos, result);
	    break;
	case 2:
#ifdef CD_SCAN_EVERYTIME
	    if (CdSearchFile (&fdvh2, VH2_FILE) == 0) {
		printf ("file not find : %s\n", VH2_FILE);
		return -1;
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdvh2.pos, result);
	    break;
	default:
	    return -1;
	    break;
	}

	read_flag ++;

#else  /* WITH_CDROM */

	read_flag += 4;

#endif /* WITH_CDROM */
	break;

#ifdef WITH_CDROM

    case 2:
	/* Wait for end of head position motion */
	FntPrint("VH: CD locate sync (%d)\n", which_read);
#ifdef CD_NOWAIT
	if (CdSync (1, result)) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdSync (0, result);
	read_flag ++;
#endif /* CD_NOWAIT */
	break;

    case 3:
	/* Indicate read in from CD */
	FntPrint("VH: CD read (%d)\n", which_read);

	switch (which_read) {
	case 1:
	    size  = fdvh1.size / SECTOR_SIZ;
	    size += ((fdvh1.size % SECTOR_SIZ) != 0) ? 1 : 0;
	    CdRead (size, (void *) VAB1_HADDR, CdlModeSpeed);
	    break;
	case 2:
	    size  = fdvh2.size / SECTOR_SIZ;
	    size += ((fdvh2.size % SECTOR_SIZ) != 0) ? 1 : 0;
	    CdRead (size, (void *) VAB2_HADDR, CdlModeSpeed);
	    break;
	}
	read_flag ++;
	break;

    case 4:
	/* Wait for end of read from CD. */
	FntPrint("VH: CD read sync (%d)\n", which_read);
#ifdef CD_NOWAIT
	if (CdReadSync (1, result) == 0) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdReadSync (0, result);
	read_flag ++;
#endif /* CD_NOWAIT */
	break;

#endif /* WITH_CDROM */

    case 5:
	/* Open VH file. */
	FntPrint("VH: open VH file (%d)\n", which_read);
	switch (which_read) {
	case 1:
	    vab1 = SsVabOpenHead ((u_char *)VAB1_HADDR, -1);
	    if (vab1 < 0) {
		printf ("SsVabOpenHead [2]: Can NOT open header [%d].\n",
			vab1);
		return -1;
	    }
	    break;
	case 2:
	    vab2 = SsVabOpenHead ((u_char *)VAB2_HADDR, -1);
	    if (vab2 < 0) {
		printf ("SsVabOpenHead [2]: Can NOT open header [%d].\n",
			vab2);
		return -1;
	    }
	    break;
	}

	read_flag ++;
#ifdef WITH_CDROM
#ifdef CD_SCAN_EVERYTIME
	vb_seek1 = 0;
#endif /* CD_SCAN_EVERYTIME */
#endif /* WITH_CDROM */
	break;

    case 6:
#ifdef WITH_CDROM
	/* Head position indication */
	FntPrint("VB: CD set locate (%d)\n", which_read);

	switch (which_read) {
	case 1:
#ifdef CD_SCAN_EVERYTIME
	    if (vb_seek1 == 0) {
		vb_seek1 ++;
		if (CdSearchFile (&fdvb1, VB1_FILE) == 0) {
		    printf ("file not find : %s\n", VB1_FILE);
		    return -1;
		}
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdvb1.pos, result);
	    break;
	case 2:
#ifdef CD_SCAN_EVERYTIME
	    if (vb_seek1 == 0) {
		vb_seek1 ++;
		if (CdSearchFile (&fdvb2, VB2_FILE) == 0) {
		    printf ("file not find : %s\n", VB2_FILE);
		    return -1;
		}
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdvb2.pos, result);
	    break;
	default:
	    return -1;
	    break;
	}

	read_flag ++;

#else  /* WITH_CDROM */
	/* Copy from memory ( jump to 10 ) */
	FntPrint("VB: memory copy (%d)\n", which_read);

	switch (which_read) {
	case 1:
	    memcpy (buf, (char *) (VAB1_BADDR + vtsize), BUFSIZE);
	    break;
	case 2:
	    memcpy (buf, (char *) (VAB2_BADDR + vtsize), BUFSIZE);
	    break;
	}
	vtsize += BUFSIZE;
	read_flag += 4;

#endif /* WITH_CDROM */
	break;

#ifdef WITH_CDROM
    case 7:
	/* Wait for end of head position motion */
	FntPrint("VB: CD locate sync (%d)\n", which_read);
#ifdef CD_NOWAIT
	if (CdSync (1, result)) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdSync (0, result);
	read_flag ++;
#endif /* CD_NOWAIT */
	break;

    case 8:
	/* Indicate read in from CD */
	FntPrint("VB: CD read (%d)\n", which_read);
	CdRead (BUFSIZE / SECTOR_SIZ, (void *) buf, CdlModeSpeed);
	read_flag ++;
	break;

    case 9:
	/* Wait for end of read from CD */
	FntPrint("VB: CD read sync (%d)\n", which_read);
#ifdef CD_NOWAIT
	if (CdReadSync (1, result) == 0) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdReadSync (0, result);
	read_flag ++;
#endif /* CD_NOWAIT */
	break;
#endif /* WITH_CDROM */

    case 10:
	/* Indicate transfer to sound buffer */
	FntPrint("VB: VAB body transition (%d)\n", which_read);
	switch (which_read) {
	case 1:
	    vab = vab1;
	    break;
	case 2:
	    vab = vab2;
	    break;
	}
	if ((vab_ = SsVabTransBodyPartly (buf, BUFSIZE, vab)) != vab) {
	    switch (vab_) {
	    case -2:		/* Continue */
		read_flag ++;
		break;
	    case -1:		/* Failed */
		printf ("SsVabTransBodyPartly [2]: failed !!!\n");
		return -1;
		break;
	    default:
		break;
	    }
	} else {
	    read_flag ++;
	}
	break;

    case 11:
	/* Wait for end of transfer event */
	FntPrint("VB: VAB body transition sync (%d)\n", which_read);
	switch (which_read) {
	case 1:
	    vab = vab1;
	    break;
	case 2:
	    vab = vab2;
	    break;
	}
	if (SsVabTransCompleted (SS_IMMEDIATE)) {
	    if (vab_ == vab) {
		/* VAB last block transfer */
		read_flag ++;
	    } else {
		/* Transfer in progress */
#ifdef WITH_CDROM
		switch (which_read) {
		case 1:
		    addLoc (&fdvb1.pos, &addsize);
		    break;
		case 2:
		    addLoc (&fdvb2.pos, &addsize);
		    break;
		}
#endif /* WITH_CDROM */
		read_flag -= 5; /* Return to "indicate head position/copy
				   from memory" */
	    }
	}

	break;

    case 12:

	/* ----------------------------------------------------------------
	 * SEQ read in */

	/* Indicate head position/copy from memory (jump to 5) */
#ifdef WITH_CDROM
	FntPrint("SEQ: CD set locate (%d)\n", which_read);

	switch (which_read) {
	case 1:
#ifdef CD_SCAN_EVERYTIME
	    if (CdSearchFile (&fdseq1, SEQ1_FILE) == 0) {
		printf ("file not find : %s\n", SEQ1_FILE);
		return -1;
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdseq1.pos, result);
	    break;
	case 2:
#ifdef CD_SCAN_EVERYTIME
	    if (CdSearchFile (&fdseq2, SEQ2_FILE) == 0) {
		printf ("file not find : %s\n", SEQ2_FILE);
		return -1;
	    }
#endif /* CD_SCAN_EVERYTIME */
	    CdControl (CdlSetloc, (u_char *)&fdseq2.pos, result);
	    break;
	default:
	    return -1;
	    break;
	}

	read_flag ++;
#else  /* WITH_CDROM */
	read_flag += 4;
#endif /* WITH_CDROM */
	break;

#ifdef WITH_CDROM

    case 13:
	/* Wait for end of head position motion. */
	FntPrint("SEQ: CD locate sync (%d)\n", which_read);
#ifdef CD_NOWAIT
	if (CdSync (1, result)) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdSync (0, result);
	read_flag ++;
#endif /* CD_NOWAIT */
	break;

    case 14:
	/* Indicate read in from CD */
	FntPrint("SEQ: CD read (%d)\n", which_read);

	switch (which_read) {
	case 1:
	    size  = fdseq1.size / SECTOR_SIZ;
	    size += ((fdseq1.size % SECTOR_SIZ) != 0) ? 1 : 0;
	    CdRead (size, (void *) SEQ1_ADDR, CdlModeSpeed);
	    break;
	case 2:
	    size  = fdseq2.size / SECTOR_SIZ;
	    size += ((fdseq2.size % SECTOR_SIZ) != 0) ? 1 : 0;
	    CdRead (size, (void *) SEQ2_ADDR, CdlModeSpeed);
	    break;
	}
	read_flag ++;
	break;

    case 15:
	/* Wait for end of read from CD */
	FntPrint("SEQ: CD read sync\n");
#ifdef CD_NOWAIT
	if (CdReadSync (1, result) == 0) {
	    read_flag ++;
	}
#else  /* CD_NOWAIT */
	CdReadSync (0, result);
	read_flag ++;

#endif /* CD_NOWAIT */
	break;
#endif /* WITH_CDROM */

    case 16:
	/* SEQ open */
	FntPrint("SEQ: open seq (%d)\n", which_read);

	switch (which_read) {
	case 1:
	    seq1 = SsSeqOpen ((u_long *)SEQ1_ADDR, vab1); /* open seq data */
	    SsSeqSetVol (seq1, SVOL, SVOL); /* set seq data volume */
	    break;
	case 2:
	    seq2 = SsSeqOpen ((u_long *)SEQ2_ADDR, vab2); /* open seq data */
	    SsSeqSetVol (seq2, SVOL, SVOL); /* set seq data volume */
	    break;
	}

	read_flag = 0;
#ifndef WITH_CDROM
	vtsize = 0;
#endif /* WITH_CDROM */
	break;
    }

    return 0;
}

void
addLoc (CdlLOC *d, CdlLOC *s)
{
    unsigned char sr1 = ((*s).sector) & 0xf;
    unsigned char sr2 = (((*s).sector) &0xf0) >> 4;
    unsigned char sd1 = ((*s).second) & 0xf;
    unsigned char sd2 = (((*s).second) &0xf0) >> 4;
    unsigned char sm1 = ((*s).minute) & 0xf;
    unsigned char sm2 = (((*s).minute) &0xf0) >> 4;

    unsigned char dr1 = ((*d).sector) & 0xf;
    unsigned char dr2 = (((*d).sector) &0xf0) >> 4;
    unsigned char dd1 = ((*d).second) & 0xf;
    unsigned char dd2 = (((*d).second) &0xf0) >> 4;
    unsigned char dm1 = ((*d).minute) & 0xf;
    unsigned char dm2 = (((*d).minute) &0xf0) >> 4;

    /* sector : rank 1 */
    dr1 += sr1;
    if (dr1 >= 10) {
	/* Move up */
	dr1 -= 10;
	dr2 ++;
    }

    /* sector : rank 10 */
    dr2 += sr2;
    if ((dr2 > 7) || ((dr2 == 7) && (dr1 >= 5))) {
	/* Move up to second */
	/* 1 second = 75 sectors */
	if (dr1 < 5) {
	    dr2 --;
	    dr1 += 10;
	}
	dr1 -= 5;
	dr2 -= 7;
	dd1 ++;			/* Move up to second */
    }

    /* second : rank 1 */
    dd1 += sd1;
    if (dd1 >= 10) {
	/* Move up */
	dd1 -= 10;
	dd2 ++;
    }

    /* second : rank 10 */
    dd2 += sd2;
    if (dd2 >= 6) {
	/* Move up to minute */
	/* 1 minute = 60 seconds */
	dd2 -= 6;
	dm1 ++;
    }

    /* minute : rank 1 */
    dm1 += sm1;
    if (dm1 >= 10) {
	dm1 -= 10;
	dm2 ++;
    }

    /* minute : rank 10 */
    dm2 += sm2;

    (*d).sector = (dr2 << 4) | dr1;
    (*d).second = (dd2 << 4) | dd1;
    (*d).minute = (dm2 << 4) | dm1;

}
#endif /* SOUND */

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
