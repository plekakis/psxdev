/* $PSLibId: Run-time Library Release 4.4$ */
/*        balls & thread:
 *
 *        Renders multiple balls bouncing on the screen,
 *        and then increments a counter during idle time
 *
 *        Copyright  (C)  1996 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   1.00      Apr.25.1996    hakama   (based balls 4.00) 
 *   -----------------------------------------
 **/

#include <r3000.h>
#include <asm.h>
#include <libapi.h>

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * Kaji Printf */
#define KANJI

/*#define DEBUG */
/*
 * Primitive Buffer */
#define OTSIZE		1		/* size of ordering table */
#define MAXOBJ		4000		/* max sprite number */
typedef struct {
	DRAWENV		draw;		/* drawing environment */
	DISPENV		disp;		/* display environment */
	u_long		ot[OTSIZE];	/* ordering table */
	SPRT_16		sprt[MAXOBJ];	/* 16x16 fixed-size sprite*/
} DB;

/*
 * Position Buffer */
typedef struct {
	u_short x, y;			/* current point */
	u_short dx, dy;			/* verocity */
} POS;

/*
 * display area */
#define	FRAME_X		320		/* frame size */
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* reflection point */
#define WALL_Y		(FRAME_Y-16)

/* preset unchanged primitve members */
static void init_prim(DB *db);	

/* parse controller */
static int  pad_read(int n);	

/* callback for VSync */
static void  cbvsync(void);	

/* intitialze position table */
static int  init_point(POS *pos);

/* declare data for thread */
#define THREAD_SAMPLE

#ifdef THREAD_SAMPLE
/* declare data for thread */
#define SUB_STACK 0x80180000 /* stack for sub-thread. update appropriately. */
static volatile unsigned long count1,count2; /* counter */
struct ToT *sysToT = (struct ToT *) 0x100 ; /* Table of Tabbles  */
struct TCBH *tcbh ; /* task status queue address */
struct TCB *master_thp,*sub_thp; /* start address of thread context */
static long sub_func() ; /* sub thread function */
#endif

main()
{
	/* buffer storing ball coordinates and displacement distance */
	POS	pos[MAXOBJ];	
	
	/* double buffer */
	DB	db[2];		
	
	/* current double buffer */
	DB	*cdb;		

	/* object number */
	int	nobj = 1;	
	
	/* current OT */
	u_long	*ot;		
	
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
#ifdef THREAD_SAMPLE
	/* thread data (local)*/
	unsigned long sub_th,gp;

	/* initialize thread */

	/* redo configuration */
	/* This operation is not usually necessary but is needed when using 2000.:
        Please refer to 2190 in the \bata\setconf directory.
        SetConf2 is replaced in version3.4 with SetConf. */
	SetConf(16,4,0x80200000);

	/* get task status queue address and current thread context address */
	tcbh = (struct TCBH *) sysToT[1].head;
	master_thp = tcbh->entry;

	/* generate sub thread */
	/* for this section, refer to library reference DTL-S2150A vol-1 p25-p29 */
	/* vol-2 first demodulator 22-second modulator 24 */
	gp = GetGp();
	EnterCriticalSection();
	sub_th = OpenTh(sub_func, SUB_STACK, gp);
	ExitCriticalSection();
	/* get sub thread context address and set interrupt status */
	sub_thp = (struct TCB *) sysToT[2].head + (sub_th & 0xffff);
	sub_thp->reg[R_SR] = 0x404;
#endif	
	/* reset controller */
	PadInit(0);		
	
	/* reset graphics system  (0: cold, 1: warm)  */
	ResetGraph(0);		
	
	/* set debug mode  (0: off, 1: monitor, 2: dump)  */
	SetGraphDebug(0);	
	
	/* set up V-sync callback function */
	VSyncCallback(cbvsync);	

	/* set up rendering/display environment for double buffering 
      when rendering (0, 0) - (320,240), display (0,240) - (320,480) (db[0]) 
      when rendering (0,240) - (320,480), display (0,  0) - (320,240) (db[1])  */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* init font environment */
#ifdef KANJI	/* KANJI */
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	/* load basic font pattern into frame buffer */
	FntLoad(960, 256);	
	
	/* set font display location */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* initialize primitive buffer */
	init_prim(&db[0]);	
	
	/* initialize primitive buffer */
	init_prim(&db[1]);	
	
	/* set initial geometries */
	init_point(pos);	

	/* enable to display */
	SetDispMask(1);		/* 0:inhibit,1:enable */

	/* main loop */
	while ((nobj = pad_read(nobj)) > 0) {
		/* swap double buffer ID */
		cdb  = (cdb==db)? db+1: db;	
#ifdef DEBUG
		/* dump DB environment */
		DumpDrawEnv(&cdb->draw);
		DumpDispEnv(&cdb->disp);
		DumpTPage(cdb->draw.tpage);
#endif

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

			/* update vertex */
			setXY0(sp, x, y);	
			
			/* apend to OT */
			AddPrim(ot, sp);	
		}
		/* wait for end of drawing */
		DrawSync(0);		
		
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
		/* cnt = VSync(0);	/* wait for V-BLNK (1/60) */
#ifdef THREAD_SAMPLE
		/* move to sub thread. sub thread begins at the point:
          where the previous interrupt obtained control (co-routine).*/
		ChangeTh(sub_th);
#else
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */
#endif		
		/* swap double buffers */
		/* update display environment */
		PutDispEnv(&cdb->disp); 
		
		/* update drawing environment */
		PutDrawEnv(&cdb->draw); 
		
		/* render primitives entered in the OT */
		DrawOTag(cdb->ot);	
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/* print the number of balls and the elapsed time */
#ifdef KANJI
		KanjiFntPrint("‹Ê‚Ì”%d\n", nobj);
#ifdef THREAD_SAMPLE
		/* display how many loops were completed before the interrupt was received */
		KanjiFntPrint("‹ó‚«ŽžŠÔ=%d\n", count2-count1);
#else
		KanjiFntPrint("ŽžŠÔ=%d\n", cnt);
#endif
		KanjiFntFlush(-1);
#endif
		
		FntPrint("sprite = %d\n", nobj);
#ifdef THREAD_SAMPLE
		/* display how many loops were completed before the interrupt was received, part 2 */
		FntPrint("free time = %d\n", count2 - count1);
#else
		FntPrint("total time = %d\n", cnt);
#endif
		FntFlush(-1);
		
#ifdef THREAD_SAMPLE
		/* save counter */
		count1 = count2;
#endif
	}
#ifdef THREAD_SAMPLE
	/* delete thread  */
	EnterCriticalSection();
	CloseTh(sub_th);
	ExitCriticalSection();
#endif
	PadStop();	/* close controller */
	StopCallback();
	return(0);
}

/*
 * Initialize drawing Primitives */
#include "balltex.h"	/* file contains ball texture pattern */

/* DB *db; primitive buffer */
static void init_prim(DB *db)
{
	u_short	clut[32];		/* CLUT entry */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */

	/* set bg color */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* load texture pattern */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifdef DEBUG
	DumpTPage(db->draw.tpage);
#endif
	/* load texture CLUT */
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
#ifdef DEBUG
		DumpClut(clut[i]);
#endif
	}

	/* initialize sprite */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		/* set SPRT_16 */
		SetSprt16(sp);		
		
		/* semi-ambient is OFF */
		SetSemiTrans(sp, 0);	
		
		/* shaded texture is OFF */
		SetShadeTex(sp, 1);	
		
		/* texture point is (0,0) */
		setUV0(sp, 0, 0);	
		
		/* set CLUT */
		sp->clut = clut[i%32];	
	}
}

/*
 * Initialize sprite position and verocity */

/* POS   *pos;          structure for ball movements */
static init_point(POS *pos)	
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* starting coordinate X */
		pos->y  = rand();		/* starting coordinate Y */
		pos->dx = (rand() % 4) + 1;	/* displacement distance X (1<=x<=4)  */
		pos->dy = (rand() % 4) + 1;	/* displacement distance Y (1<=y<=4)  */
		pos++;
	}
}

/*
 * Read control-pad */
/* int n; number of sprites */
static int pad_read(int	n)		
{
	u_long	padd = PadRead(1);		/* read controller */

	if(padd & PADLup)	n += 4;		/* up arrow key on the left of the controller */
	if(padd & PADLdown)	n -= 4;		/* left arrow key on the left of the controller */

	if (padd & PADL1) 			/* pause */
		while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return(-1);	/* end of program */

	limitRange(n, 1, MAXOBJ-1);	/* set n to 1<=n<= (MAXOBJ-1) */
					/* see libgpu.h */
	return(n);
}

/*
 * callback */

static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));
#ifdef THREAD_SAMPLE
	/* return from interrupt set to main thread. if this is not done, control will
        return to sub thread (in sub_func()).  */
	tcbh->entry = master_thp;
#endif
}

#ifdef THREAD_SAMPLE
/* program to loop and count up during idle time.
   note that functions called from ChangeTh() will not have anywhere to return to.  */
static long sub_func()
{
  count1 = 0;
  count2 = 0;
  while(1){
    /* A Vsync interrupt is received somewhere in this while loop, and control is taken away.
       Control resumes from there at the next ChangeTh(). */
    count2 ++;
  }
}
#endif
