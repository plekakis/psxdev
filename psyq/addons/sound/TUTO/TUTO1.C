/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto1.c,v $
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
static char rcsid [] = "$Id: tuto1.c,v 1.5 1996/05/21 07:59:54 hatto Exp $ : \
	Copyright (C) by 1994 Sony Computer Entertainment Inc.";
#endif

#include <r3000.h>
#include <asm.h>
#include <kernel.h>

#include <sys/types.h>
#include <libetc.h>
#include <libspu.h>
#ifdef __psx__
#include <libsn.h>
#else  /* __psx__ */
#define pollhost()
#endif /* __psx__ */

/* #define DEBUG /**/
/* #define TRANSFERRED_BY_IO /**/

#ifdef DEBUG
#define PRINTF(x) printf x
#else
#define PRINTF(x)
#endif

/*
 * waveform data of sin curve
 */
#define SIN_DATA_SIZE (0x10 * 10)
unsigned char sin_wave [] = {
#include "sin.h"
};

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

void keyOn (int);
void keyOff (int);
void padHandle (unsigned long);

int quitF;

#define MALLOC_MAX 1
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

main (void)
{
    unsigned long addr, a_addr;
    unsigned long size;
    SpuVoiceAttr s_attr;
    SpuCommonAttr c_attr;

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
     *		SPU の初期化 /
     *		Initialize SPU
     * ---------------------------------------------------------------- */

    SpuInit ();
    SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);

    /* ----------------------------------------------------------------
     *		コントロールパッド の初期化 /
     *		Initialize Control pad 
     * ---------------------------------------------------------------- */

    PadInit (0);

    /* ----------------------------------------------------------------
     *		共通属性設定 /
     *		set common attributes (Master volume, and so on).
     * ---------------------------------------------------------------- */

    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR);

    c_attr.mvol.left  = 0x3fff;	/* Master volume (left) */
    c_attr.mvol.right = 0x3fff;	/* Master volume (right) */

    SpuSetCommonAttr (&c_attr);

    /* ----------------------------------------------------------------
     *		波形転送 /
     *		transfer waveform data to SPU sound-buffer.
     * ---------------------------------------------------------------- */

    /* 転送モード (設定は DMA 転送) /
       Transfer mode (set DMA mode) */

#ifdef TRANSFERRED_BY_IO
    SpuSetTransferMode (SpuTransByIO); /* transfer by I/O */
#else
    SpuSetTransferMode (SpuTransByDMA); /* transfer by DMA */
#endif /* TRANSFERRED_BY_IO */

#ifdef DEBUG
    PRINTF (("Transfer waveform data "));
    switch (SpuGetTransferMode ()) {
    case SpuTransferByDMA:
	PRINTF (("by DMA\n"));
	break;
    case SpuTransferByIO:
	PRINTF (("by IO\n"));
	break;
    default:
	PRINTF (("???\n"));
	break;
    }
#endif /* DEBUG */


    a_addr = SpuMalloc (SIN_DATA_SIZE);

    /* 転送先頭アドレス設定 /
     * setting of start address that waveform data is transfered */

    addr = SpuSetTransferStartAddr (a_addr);
    PRINTF (("\tSet start addr    : %08x\n", a_addr));
    PRINTF (("\tReturn value      : %08x\n", addr));
    PRINTF (("\tGet start addr    : %08x\n", SpuGetTransferStartAddr ()));
    
    /* 転送
     * transfer waveform data to SPU sound-buffer. */

    size = SpuWrite (sin_wave, SIN_DATA_SIZE);

#ifndef TRANSFERRED_BY_IO
    while (! SpuIsTransferCompleted (SPU_TRANSFER_GLANCE)) {
	PRINTF (("."));
    }
#endif /* TRANSFERRED_BY_IO */

    PRINTF (("\tSend size    : %08x\n", SIN_DATA_SIZE));
    PRINTF (("\tReturn value : %08x\n", size));

    /* ----------------------------------------------------------------
     *		ボイス属性設定 /
     *		setting of each voice attributes.
     * ---------------------------------------------------------------- */

    /* ボイス属性項目 / items of voice attribute */
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

    /* 全ボイス設定 / set the attributes with all voice */
    s_attr.voice = SPU_ALLCH;

#define S_PITCH 0x1000

    /* 属性値 / value of each voice attribute */
    s_attr.volume.left  = 0x3fff;		/* Left volume */
    s_attr.volume.right = 0x3fff;		/* Right volume */
    s_attr.pitch        = S_PITCH;		/* Pitch */
    s_attr.addr         = a_addr;		/* Waveform data
						   start address */
    s_attr.a_mode       = SPU_VOICE_LINEARIncN;	/* Attack curve */
    s_attr.s_mode       = SPU_VOICE_LINEARIncN;	/* Sustain curve */
    s_attr.r_mode       = SPU_VOICE_LINEARDecN;	/* Release curve */
    s_attr.ar           = 0x0;			/* Attack rate value */
    s_attr.dr           = 0x0;			/* Decay rate value */
    s_attr.sr           = 0x0;			/* Sustain rate value */
    s_attr.rr           = 0x0;			/* Release rate value */
    s_attr.sl           = 0xf;			/* Sustain level value */

    SpuSetVoiceAttr (&s_attr);

    quitF = False;
    while (! quitF) {
	VSync (0);
#if 0
	pollhost ();
#endif
	padHandle (PadRead (0));
    }

    /* ----------------------------------------------------------------
     *		終了処理 /
     *		Finalize this sample
     * ---------------------------------------------------------------- */

    SpuSetKey (SpuOff, SPU_ALLCH);
    SpuQuit ();

    PadStop ();		/* pad stop */

    ResetGraph (3);
    StopCallback ();

    return;
}

void
keyOff (int ch)
{
    PRINTF (("Key Off : %d ch\n", ch));
    SpuSetKey (SpuOff, SPU_VOICECH(ch));
}

void
keyOn (int ch)
{
    SpuVoiceAttr s_attr;
    s_attr.mask = SPU_VOICE_PITCH;

    PRINTF (("Key On : %d ch\n", ch));

    s_attr.voice = SPU_VOICECH(ch);
    s_attr.pitch = S_PITCH + (S_PITCH * ch / 12);
    SpuSetKeyOnWithAttr (&s_attr);
}

void
padHandle (unsigned long padd)
{
    static int key_Rup    = False;
    static int key_Rdown  = False;
    static int key_Rleft  = False;
    static int key_Rright = False;
    static int key_Lup    = False;
    static int key_Ldown  = False;
    static int key_Lright = False;
    static int key_Lleft  = False;

    if (padd & PADLleft) {
	if ((key_Lleft == False)) {
	    keyOn (0);
	    key_Lleft = True;
	}
    } else {
	if (key_Lleft == True) {
	    keyOff (0);
	    key_Lleft = False;
	}
    }

    if (padd & PADLup) {
	if ((key_Lup == False)) {
	    keyOn (1);
	    key_Lup = True;
	}
    } else {
	if ((key_Lup == True)) {
	    keyOff (1);
	    key_Lup = False;
	}
    }

    if (padd & PADLright) {
	if ((key_Lright == False)) {
	    keyOn (2);
	    key_Lright = True;
	}
    } else {
	if ((key_Lright == True)) {
	    keyOff (2);
	    key_Lright = False;
	}
    }

    if (padd & PADLdown) {
	if ((key_Ldown == False)) {
	    keyOn (3);
	    key_Ldown = True;
	}
    } else {
	if ((key_Ldown == True)) {
	    keyOff (3);
	    key_Ldown = False;
	}
    }

    if (padd & PADRleft) {
	if ((key_Rleft == False)) {
	    keyOn (4);
	    key_Rleft = True;
	}
    } else {
	if ((key_Rleft == True)) {
	    keyOff (4);
	    key_Rleft = False;
	}
    }

    if (padd & PADRup) {
	if ((key_Rup == False)) {
	    keyOn (5);
	    key_Rup = True;
	}
    } else {
	if ((key_Rup == True)) {
	    keyOff (5);
	    key_Rup = False;
	}
    }

    if (padd & PADRright) {
	if ((key_Rright == False)) {
	    keyOn (6);
	    key_Rright = True;
	}
    } else {
	if ((key_Rright == True)) {
	    keyOff (6);
	    key_Rright = False;
	}
    }

    if (padd & PADRdown) {
	if ((key_Rdown == False)) {
	    keyOn (7);
	    key_Rdown = True;
	}
    } else {
	if ((key_Rdown == True)) {
	    keyOff (7);
	    key_Rdown = False;
	}
    }

    if (padd & PADk) {
	PRINTF (("Quit\n"));
	quitF = True;
    }
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
