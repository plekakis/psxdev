/* $PSLibId: Run-time Library Release 4.0$ */
#if defined(APD_SAVE) || defined(APD_LOAD)
	static char	*progname="graphics/fballs/fballs.cpe";
#endif
/*				Fast balls
 *
 *		‰æ–Ê“à‚ğƒoƒEƒ“ƒh‚·‚é•¡”‚Ìƒ{[ƒ‹‚ğ•`‰æ‚·‚é
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/*
 * definition for speed up: ‚‘¬‰»‚Ì‚½‚ß‚Ì’è‹` 
 */
#define FASTVBLNK	/* shorten VBLNK: VBLNK ‚ğ’Z‚­‚·‚é */
#define SMALLDRAWAREA	/* decrease drawing: •`‰æ‚ğŒ¸‚ç‚· */
//#define OTENV		/* do PutDrawEnv and DrawOTag at the same time
			  // : PutDrawEnv ‚Æ DrawOTag ‚ğ“¯‚És‚¤ */

/*
 * Kaji Printf: Š¿š‚ğƒvƒŠƒ“ƒg‚·‚é‚½‚ß‚Ì’è‹`
 */
#define KANJI

/*#define DEBUG */
/*
 * Primitive Buffer: ƒvƒŠƒ~ƒeƒBƒuŠÖ˜A‚Ìƒoƒbƒtƒ@
 */
#define OTSIZE		1		/* size of ordering table
					   : ƒI[ƒ_ƒŠƒ“ƒOƒe[ƒuƒ‹‚Ì” */
#define MAXOBJ		4000		/* max sprite number :
					   ƒXƒvƒ‰ƒCƒgiƒ{[ƒ‹j”‚ÌãŒÀ */
typedef struct {
	DRAWENV		draw;		/* drawing environment : •`‰æŠÂ‹« */
	DISPENV		disp;		/* display environment : •\¦ŠÂ‹« */
	u_long		ot[OTSIZE];	/* ordering table:
					   : ƒI[ƒ_ƒŠƒ“ƒOƒe[ƒuƒ‹ */
	SPRT_16		sprt[MAXOBJ];	/* 16x16 fixed-size sprite:
					   16x16ŒÅ’èƒTƒCƒY‚ÌƒXƒvƒ‰ƒCƒg */
} DB;

/*
 * Position Buffer: ƒXƒvƒ‰ƒCƒg‚Ì“®‚«‚ÉŠÖ‚·‚éƒoƒbƒtƒ@
 */
typedef struct {
	u_short x, y;			/* current point: Œ»İ‚ÌˆÊ’u */
	u_short dx, dy;			/* verocity: ‘¬“x */
} POS;

/*
 * •\¦—Ìˆæ
 */
#define	FRAME_X		640		/* frame size:•\¦—ÌˆæƒTƒCƒY(320x240)*/
#define	FRAME_Y		480
#define WALL_X		(FRAME_X-16)	/* reflection point
					   : ƒXƒvƒ‰ƒCƒg‚Ì‰Â“®—ÌˆæƒTƒCƒY */
#define WALL_Y		(FRAME_Y-16)

/* preset unchanged primitve members: ƒvƒŠƒ~ƒeƒBƒuƒoƒbƒtƒ@‚Ì‰Šúİ’è */
static void init_prim(DB *db);	

/* parse controller: ƒRƒ“ƒgƒ[ƒ‰‚Ì‰ğÍ */
static int  pad_read(int n);	

/* callback for VSync: V-Sync‚ÌƒR[ƒ‹ƒoƒbƒNƒ‹[ƒ`ƒ“ */
static void  cbvsync(void);	

/* intitialze position table : ƒ{[ƒ‹‚ÌƒXƒ^[ƒgˆÊ’u‚ÆˆÚ“®‹——£‚Ìİ’è */
static int  init_point(POS *pos);

main()
{
	/* ƒ{[ƒ‹‚ÌÀ•W’l‚ÆˆÚ“®‹——£‚ğŠi”[‚·‚éƒoƒbƒtƒ@ */
	POS	pos[MAXOBJ];	
	
	/* double buffer: ƒ_ƒuƒ‹ƒoƒbƒtƒ@‚Ì‚½‚ß‚Q‚Â—pˆÓ‚·‚é */
	DB	db[2];		
	
	/* current double buffer: Œ»İ‚Ìƒ_ƒuƒ‹ƒoƒbƒtƒ@ƒoƒbƒtƒ@‚ÌƒAƒhƒŒƒX */
	DB	*cdb;		

	/* object number: •\¦‚·‚éƒXƒvƒ‰ƒCƒg‚Ì”iÅ‰‚Í‚P‚Â‚©‚çj*/
	int	nobj = 1;	
	
	/* current OT: Œ»İ‚Ì‚n‚s‚ÌƒAƒhƒŒƒX */
	u_long	*ot;		
	
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */

	/* reset PAD: ƒRƒ“ƒgƒ[ƒ‰‚ÌƒŠƒZƒbƒg */
#if defined(APD_SAVE)
	APDSaveInit(0,progname);
#elif defined(APD_LOAD)
	if ( APDLoadInit(0,progname) ) return 0;
#endif
#ifndef APD_LOAD
	PadInit(0);		
#endif
	
	/* reset graphics sysmtem (0:cold,1:warm); •`‰æE•\¦ŠÂ‹«‚ÌƒŠƒZƒbƒg */
	ResetGraph(0);		
	
	/* set debug mode (0:off,1:monitor,2:dump): ƒfƒoƒbƒOƒ‚[ƒh‚Ìİ’è */
	SetGraphDebug(0);	
	
	/* set callback: V-sync‚ÌƒR[ƒ‹ƒoƒbƒNŠÖ”‚Ìİ’è */
	VSyncCallback(cbvsync);	

	/* inititlalize environment for double buffer
	: •`‰æE•\¦ŠÂ‹«‚ğƒ_ƒuƒ‹ƒoƒbƒtƒ@—p‚Éİ’è
	(0,  0)-(320,240)‚É•`‰æ‚µ‚Ä‚¢‚é‚Æ‚«‚Í(0,240)-(320,480)‚ğ•\¦(db[0])
	(0,240)-(320,480)‚É•`‰æ‚µ‚Ä‚¢‚é‚Æ‚«‚Í(0,  0)-(320,240)‚ğ•\¦(db[1])
	*/
#ifdef SMALLDRAWAREA
	{
		RECT rect;

		setRECT(&rect, 0, 0, 640, 512);
		ClearImage(&rect, 0, 0, 0);
	}
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 464);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 464);
#else /* SMALLDRAWAREA */
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
#endif /* SMALLDRAWAREA */
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);
#ifdef FASTVBLNK
	db[0].disp.screen.h = 242;
	db[1].disp.screen.h = 242;
#endif /* FASTVBLNK */

	/* init font environment; ƒtƒHƒ“ƒg‚Ìİ’è */
#ifdef KANJI	/* KANJI */
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	/* :Šî–{ƒtƒHƒ“ƒgƒpƒ^[ƒ“‚ğƒtƒŒ[ƒ€ƒoƒbƒtƒ@‚Éƒ[ƒh */
	FntLoad(960, 256);	
	
	/* :ƒtƒHƒ“ƒg‚Ì•\¦ˆÊ’u‚Ìİ’è */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* initialize primitive buffer: ƒvƒŠƒ~ƒeƒBƒuƒoƒbƒtƒ@‚Ì‰Šúİ’è(db[0])*/
	init_prim(&db[0]);	
	
	/* initialize primitive buffer: ƒvƒŠƒ~ƒeƒBƒuƒoƒbƒtƒ@‚Ì‰Šúİ’è(db[1])*/
	init_prim(&db[1]);	
	
	/* set initial geometries: ƒ{[ƒ‹‚Ì“®‚«‚ÉŠÖ‚·‚é‰Šúİ’è */
	init_point(pos);	

	/* enable to display: ƒfƒBƒXƒvƒŒƒC‚Ö‚Ì•\¦ŠJn */
	SetDispMask(1);		/* 0:inhibit,1:enable: ‚OF•s‰Â  ‚PF‰Â */

	/* ; ƒƒCƒ“ƒ‹[ƒv */
	while ((nobj = pad_read(nobj)) > 0) {
		/* swap double buffer ID: ƒ_ƒuƒ‹ƒoƒbƒtƒ@ƒ|ƒCƒ“ƒ^‚ÌØ‚è‘Ö‚¦ */
		cdb  = (cdb==db)? db+1: db;	
#ifdef DEBUG
		/* dump DB environment */
		DumpDrawEnv(&cdb->draw);
		DumpDispEnv(&cdb->disp);
		DumpTPage(cdb->draw.tpage);
#endif

 		/* clear ordering table: ƒI[ƒ_ƒŠƒ“ƒOƒe[ƒuƒ‹‚ÌƒNƒŠƒA */
		ClearOTag(cdb->ot, OTSIZE);

		/* update sprites :
		   ƒ{[ƒ‹‚ÌˆÊ’u‚ğ‚P‚Â‚¸‚ÂŒvZ‚µ‚Ä‚n‚s‚É“o˜^‚µ‚Ä‚¢‚­ */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* detect reflection:
			   À•W’l‚ÌXV‚¨‚æ‚Ñ‰æ–Êã‚Å‚ÌˆÊ’u‚ÌŒvZ */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* update vertex: ŒvZ‚µ‚½À•W’l‚ğƒZƒbƒg */
			setXY0(sp, x, y);	
			
			/* apend to OT: ‚n‚s‚Ö“o˜^ */
			AddPrim(ot, sp);	
		}
		/* wait for end of drawing: •`‰æ‚ÌI—¹‘Ò‚¿ */
		DrawSync(0);		
		
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
#ifdef APD_LOAD
		cnt = APDSetCnt(VSync(0));
#else
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */
#endif
		/*: ƒ_ƒuƒ‹ƒoƒbƒtƒ@‚ÌØ‘Ö‚¦ */
		/* update display environment: •\¦ŠÂ‹«‚ÌXV */
		PutDispEnv(&cdb->disp); 
		
		/* update drawing environment: •`‰æŠÂ‹«‚ÌXV */
#ifdef OTENV
		DrawOTagEnv(cdb->ot, &cdb->draw); 
#else /* OTENV */
		PutDrawEnv(&cdb->draw); 
		
		/*: ‚n‚s‚É“o˜^‚³‚ê‚½ƒvƒŠƒ~ƒeƒBƒu‚Ì•`‰æ */
		DrawOTag(cdb->ot);	
#endif /* OTENV */
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/*: ƒ{[ƒ‹‚Ì”‚ÆŒo‰ßŠÔ‚ÌƒvƒŠƒ“ƒg */
#ifdef KANJI
		KanjiFntPrint("‹Ê‚Ì”%d\n", nobj);
		KanjiFntPrint("ŠÔ=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n", cnt);
		FntFlush(-1);
	}
#ifndef APD_LOAD
	
    PadStop();	/*: ƒRƒ“ƒgƒ[ƒ‰‚ÌƒNƒ[ƒY */
#endif
#if defined(APD_SAVE)
	APDSaveStop();
#elif defined(APD_LOAD)
	APDLoadStop(VSync(-1));
#endif
	ResetGraph(3);
    StopCallback();
	return(0);
}

/*
 * Initialize drawing Primitives: ƒvƒŠƒ~ƒeƒBƒuƒoƒbƒtƒ@‚Ì‰Šúİ’è
 */
#include "balltex.h"	/* ƒ{[ƒ‹‚ÌƒeƒNƒXƒ`ƒƒƒpƒ^[ƒ“‚ª“ü‚Á‚Ä‚¢‚éƒtƒ@ƒCƒ‹ */

/* DB *db; ƒvƒŠƒ~ƒeƒBƒuƒoƒbƒtƒ@*/
static void init_prim(DB *db)
{
	u_short	clut[32];		/* CLUT entry: ƒeƒNƒXƒ`ƒƒ CLUT */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */

	/* set bg color: ”wŒiF‚ÌƒZƒbƒg */
	db->draw.isbg = 1;
   setRGB0(&db->draw, rand()%64, rand()%64, rand()%64); 

	/* load texture pattern: ƒeƒNƒXƒ`ƒƒ‚Ìƒ[ƒh */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifdef DEBUG
	DumpTPage(db->draw.tpage);
#endif
	/* load texture CLUT: ƒeƒNƒXƒ`ƒƒ CLUT‚Ìƒ[ƒh */
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 640, 480+i);
#ifdef DEBUG
		DumpClut(clut[i]);
#endif
	}

	/* initialize sprite: ƒXƒvƒ‰ƒCƒg‚Ì‰Šú‰» */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		/* set SPRT_16: 16x16ƒXƒvƒ‰ƒCƒgƒvƒŠƒ~ƒeƒBƒu‚Ì‰Šú‰» */
		SetSprt16(sp);		
		
		/* semi-ambient is ON: ”¼“§–¾‘®«ƒ ONt */
		SetSemiTrans(sp, 1);	
		
		/* shaded texture is OFF: ƒVƒF[ƒfƒBƒ“ƒO‚ğs‚í‚È‚¢ */
		SetShadeTex(sp, 1);	
		
		/* texture point is (0,0): u,v‚ğ(0,0)‚Éİ’è */
		setUV0(sp, 0, 0);	
		
		/* set CLUT: CLUT ‚ÌƒZƒbƒg */
		sp->clut = clut[i%32];	
	}
}

/*
 * Initialize sprite position and verocity:
 : ƒ{[ƒ‹‚ÌƒXƒ^[ƒgˆÊ’u‚ÆˆÚ“®—Ê‚ğİ’è‚·‚é
 */

/* POS	*pos;		ƒ{[ƒ‹‚Ì“®‚«‚ÉŠÖ‚·‚é\‘¢‘Ì */
static init_point(POS *pos)	
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/*: ƒXƒ^[ƒgÀ•W ‚w */
		pos->y  = rand();		/*: ƒXƒ^[ƒgÀ•W ‚x */
		pos->dx = (rand() % 4) + 1;	/*: ˆÚ“®‹——£ ‚w (1<=x<=4) */
		pos->dy = (rand() % 4) + 1;	/*: ˆÚ“®‹——£ ‚x (1<=y<=4) */
		pos++;
	}
}

/*
 * Read controll-pad: ƒRƒ“ƒgƒ[ƒ‰‚Ì‰ğÍ
 */
/* int n; ƒXƒvƒ‰ƒCƒg‚Ì” */
static int pad_read(int	n)		
{
	u_long	padd;

	/*: ƒRƒ“ƒgƒ[ƒ‰‚Ì“Ç‚İ‚İ */
#if defined(APD_SAVE)
	padd = APDSave(PadRead(1));
#elif defined(APD_LOAD)
	padd = APDLoad(1);
#else
	padd = PadRead(1);
#endif

	if(padd & PADLup)	n += 4;		/*: ¶‚Ì\šƒL[ƒAƒbƒv */
	if(padd & PADLdown)	n -= 4;		/*: ¶‚Ì\šƒL[ƒ_ƒEƒ“ */

	if (padd & PADL1) 			/*: pause */
#if defined(APD_SAVE)
		while (APDSave(PadRead(1))&PADL1);
#elif defined(APD_LOAD)
		while (APDLoad(1)&PADL1);
#else
		while (PadRead(1)&PADL1);
#endif

	if(padd & PADselect) 	return(-1);	/*: ƒvƒƒOƒ‰ƒ€‚ÌI—¹ */

	limitRange(n, 1, MAXOBJ-1);	/*: n‚ğ1<=n<=(MAXOBJ-1)‚Ì’l‚É‚·‚é */
					/* see libgpu.h: libgpu.h‚É‹LÚ */
	return(n);
}

/*
 * callback: ƒR[ƒ‹ƒoƒbƒN
 */
static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));	
}


