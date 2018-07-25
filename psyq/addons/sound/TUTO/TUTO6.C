/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto6.c,v $
 *
 * Copyright (C) 1994 by Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/
/*
 * $PSLibId: Runtime Library Release 3.6$
 */

#ifndef lint
static char rcsid [] = "$Id: tuto6.c,v 1.5 1996/05/21 08:00:02 hatto Exp $ : \
	Copyright (C) by 1994 Sony Computer Entertainment Inc.";
#endif

#include <sys/types.h>

#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libspu.h>
#ifdef __psx__
#include <libsn.h>
#else
#define pollhost()
#endif

/* #define DEBUG /**/

#ifdef DEBUG
#define PRINTF(x) printf x
#else
#define PRINTF(x)
#endif

int quitF;

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

#define PF_DATA_SIZE 18704
unsigned char pf_wave [] = {
#include "pf_wave.h"
};

SpuIRQCallbackProc spu_intr (void);

void padHandle (unsigned long);

#define MALLOC_MAX 2
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

char *rev_name [] = {
    "OFF",
    "ROOM",
    "STUDIO A",
    "STUDIO B",
    "STUDIO C",
    "HALL",
    "SPACE",
    "ECHO",
    "DELAY",
    "PIPE"
};

long rev_mode;
SpuReverbAttr r_attr, r_attr;

main (void)
{
    DRAWENV draw;		/* drawing environment */
    DISPENV disp;		/* display environment */

    unsigned long p_addr;
    SpuVoiceAttr s_attr;
    SpuCommonAttr c_attr;

    long top;
    long statV;

    /* ----------------------------------------------------------------
     *		割り込み環境の初期化 /
     *		Initialize interrupt environment.
     * ---------------------------------------------------------------- */

    ResetCallback();

    /* ----------------------------------------------------------------
     *		グラフィックスの初期化 /
     *		Initialize Graphics system
     * ---------------------------------------------------------------- */

    ResetGraph (0);

    /* ----------------------------------------------------------------
     *		コントロールパッド の初期化 /
     *		Initialize Control pad 
     * ---------------------------------------------------------------- */

    PadInit (0);

    /* ----------------------------------------------------------------
     *		グラフィックスモードの設定
     *		:Set graphics mode 
     * ---------------------------------------------------------------- */

    /* initialize environment for double buffer */
    SetDefDrawEnv (&draw, 0, 0, 320, 240);
    SetDefDispEnv (&disp, 0, 0, 320, 240);

    /* init font environment */
    FntLoad (960, 256);
    SetDumpFnt (FntOpen(0, 0, 256, 200, 0, 512));
    ClearImage ((RECT*)&disp, 0, 0, 0);

    /* display */
    SetDispMask (1);    /* enable to display (0:inhibit, 1:enable) */
    PutDispEnv (&disp); /* update display environment */
    PutDrawEnv (&draw); /* update drawing environment */

    /* ----------------------------------------------------------------
     *		SPU の初期化 /
     *		Initialize SPU
     * ---------------------------------------------------------------- */

    SpuInit ();
    SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);

    /* ----------------------------------------------------------------
     *		共通属性設定
     *		:Set common attributes
     * ---------------------------------------------------------------- */

    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR);

    c_attr.mvol.left  = 0x3fff;
    c_attr.mvol.right = 0x3fff;

    SpuSetCommonAttr (&c_attr);

    /* ----------------------------------------------------------------
     *		波形転送
     *		:Transfer waveforms
     * ---------------------------------------------------------------- */

    /* 転送モード */
    /* :Transform mode */
    SpuSetTransferMode (SpuTransByDMA);

    /*
     *		ピアノ音の転送
     *		:Transfer piano sound
     */

    p_addr = SpuMalloc (PF_DATA_SIZE);

    /* 転送先頭アドレス */
    /* :Transfer starting address */
    top = SpuSetTransferStartAddr (p_addr);
    PRINTF (("Set start addr    : %08x\n", p_addr));
    PRINTF (("Return start addr : %08x\n", top));
    PRINTF (("Get start addr    : %08x\n", SpuGetTransferStartAddr ()));
    
    /* 転送 */
    /* :Transfer starting address */
    SpuWrite ((unsigned char *) pf_wave, PF_DATA_SIZE);
    SpuIsTransferCompleted (SPU_TRANSFER_WAIT);

    /* ----------------------------------------------------------------
     *		ボイス属性設定
     *		:Set voice attributes
     * ---------------------------------------------------------------- */

    /* ボイス属性 */
    /* :Voice attributes */
    s_attr.mask = (SPU_VOICE_VOLL |
		   SPU_VOICE_VOLR |
		   SPU_VOICE_PITCH |
		   SPU_VOICE_WDSA |
		   SPU_VOICE_ADSR_AMODE |
		   SPU_VOICE_ADSR_SMODE |
		   SPU_VOICE_ADSR_RMODE |
		   SPU_VOICE_ADSR_AR |
		   SPU_VOICE_ADSR_DR |
		   SPU_VOICE_ADSR_SR |
		   SPU_VOICE_ADSR_RR |
		   SPU_VOICE_ADSR_SL
		   );

    /* ピアノ音 */
    /* :Piano sound */
    s_attr.voice = (SPU_0CH);

    s_attr.volume.left  = 0x1fff;
    s_attr.volume.right = 0x1fff;
    s_attr.pitch        = 0x1000;
    s_attr.addr         = p_addr;
    s_attr.a_mode       = SPU_VOICE_LINEARIncN;
    s_attr.s_mode       = SPU_VOICE_LINEARIncN;
    s_attr.r_mode       = SPU_VOICE_LINEARDecN;
    s_attr.ar           = 0x0;
    s_attr.dr           = 0x0;
    s_attr.sr           = 0x0;
    s_attr.rr           = 0x0;
    s_attr.sl           = 0xf;

    PRINTF (("set voice attr (piano):\n"));
    SpuSetVoiceAttr (&s_attr);

    /* ----------------------------------------------------------------
     *		リバーブモード
     *		:Reverb mode
     * ---------------------------------------------------------------- */

    rev_mode = SPU_REV_MODE_OFF;

    r_attr.mask        = (SPU_REV_MODE |
			  SPU_REV_DEPTHL | SPU_REV_DEPTHR);
    r_attr.mode        = (SPU_REV_MODE_CLEAR_WA | rev_mode);
    r_attr.depth.left  = 0x3fff;
    r_attr.depth.right = 0x3fff;

    SpuSetReverbModeParam (&r_attr);
    SpuSetReverbDepth (&r_attr);
    SpuSetReverbVoice (SpuOn, SPU_0CH);

    SpuSetReverb (SpuOn);

    /* ----------------------------------------------------------------
     *		発音
     *		:Key ON
     * ---------------------------------------------------------------- */

    quitF = False;
    while (! quitF) {

	statV = SpuGetKeyStatus (SPU_0CH);

	FntPrint ("\n\n\n\n\tLibspu test program: test6.c\n");

	FntPrint ("\n\n");

	FntPrint ("\tReverb\n\tMode : [%d | %s]\n\n", rev_mode, rev_name [rev_mode]);

	FntPrint ("\tKey On/Off Status : %s\n",
		  (statV == SpuOn)    ? "ON" :
		  (statV == SpuOff)   ? "OFF" :
		  (statV == SpuOffEnvOn) ? "Env ON" : "Env OFF"
		  );

	VSync (0);
#if 0
	pollhost ();
#endif
	padHandle (PadRead (0));
	ClearImage ((RECT*)&disp, 0, 0, 0 );
	FntFlush (-1);
    }

    /* ----------------------------------------------------------------
     *		終了処理 /
     *		Finalize this sample
     * ---------------------------------------------------------------- */

    VSync (0);
    ClearImage ((RECT*)&disp, 0, 0, 0);
    FntFlush (-1);

    SpuSetKey (SPU_OFF, SPU_ALLCH);

    r_attr.mode = (SPU_REV_MODE_CLEAR_WA | SPU_REV_MODE_OFF);
    r_attr.depth.left  = 0;
    r_attr.depth.right = 0;
    SpuSetReverbModeParam (&r_attr);

    SpuQuit ();

    PadStop ();		/* pad stop */

    ResetGraph (3);
    StopCallback ();

    return;
}

void
padHandle (unsigned long padd)
{
    static int key_h = False;
    static int key_k = False;
    static int key_lu = False;
    static int key_ld = False;

    /* 終了 */
    /* :Exit  */
    if ((padd & PADk) &&
	(padd & PADh)) {
	quitF = True;
	return;
    }

    /* Play */
    if (padd & PADh) {
	if ((key_h == False)) {
	    SpuSetKey (SpuOn, SPU_0CH);
	    key_h = True;
	}
    } else {
	if (key_h == True) {
	    key_h = False;
	}
    }

    /* Stop */
    if (padd & PADk) {
	if ((key_k == False)) {
	    SpuSetKey (SpuOff, SPU_0CH);
	    key_k = True;
	}
    } else {
	if (key_k == True) {
	    key_k = False;
	}
    }

    /* Reverb mode Up */
    if (padd & PADLup) {
	if ((key_lu == False)) {
	    if (rev_mode < (SPU_REV_MODE_MAX - 1)) {
		rev_mode ++;
		r_attr.mode = (SPU_REV_MODE_CLEAR_WA | rev_mode);
		SpuSetReverbModeParam (&r_attr);
		SpuSetReverbDepth (&r_attr);
	    }
	    key_lu = True;
	}
    } else {
	if (key_lu == True) {
	    key_lu = False;
	}
    }

    /* Reverb mode Down */
    if (padd & PADLdown) {
	if ((key_ld == False)) {
	    if (rev_mode > 0) {
		rev_mode --;
		r_attr.mode = (SPU_REV_MODE_CLEAR_WA | rev_mode);
		SpuSetReverbModeParam (&r_attr);
		SpuSetReverbDepth (&r_attr);
	    }
	    key_ld = True;
	}
    } else {
	if (key_ld == True) {
	    key_ld = False;
	}
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
