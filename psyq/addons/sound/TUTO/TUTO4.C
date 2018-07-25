/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto4.c,v $
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
static char rcsid [] = "$Id: tuto4.c,v 1.5 1996/05/21 07:59:59 hatto Exp $ : \
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
#else
#define pollhost()
#endif

/* #define DEBUG /**/
/* #define TRANSFERRED_BY_IO /**/

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

/*
 * waveform data of sin curve
 */
#define SIN_DATA_SIZE (0x10 * 10)
unsigned char sin_wave [] = {
#include "sin.h"
};

#define VOICE_CH 0
#define NOISE_CH 1

void keyOn (int);
void keyOff (void);
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
     *		非同期転送の設定 /
     *		setting of asynchronous transfer to SPU sound-buffer.
     * ---------------------------------------------------------------- */

    ResetGraph (0); /* グラフィックス系の初期化
		     * Initialize Graphics system */

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

#define S_ADDR 0x1000

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

    s_attr.voice = SPU_ALLCH;

#define S_PITCH 0x1000

    /* 属性値 / value of each voice attribute */
    s_attr.volume.left  = 0x1fff;		/* Left volume */
    s_attr.volume.right = 0x1fff;		/* Right volume */
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

    PRINTF (("set voice attr ... "));
    SpuSetVoiceAttr (&s_attr);

    PRINTF (("set noise attr ... "));
    SpuSetNoiseVoice (SpuOn, SPU_VOICECH(NOISE_CH));

    PRINTF (("Noise : %08x\n", SpuGetNoiseVoice ()));

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

    SpuSetKey (SPU_OFF, SPU_VOICECH(NOISE_CH));
    SpuSetNoiseVoice (SPU_OFF, SPU_VOICECH(NOISE_CH));

    SpuQuit ();

    PadStop ();		/* pad stop */

    ResetGraph (3);
    StopCallback ();

    return;
}

/* Normal Voice Key on */
void
keyOn (int value)
{
    SpuVoiceAttr s_attr;

#ifdef DEBUG
    if (value == 0 || value == 0x3fff) {
	PRINTF (("Normal voice : Key On\n"));
    }
#endif /* DEBUG */

    s_attr.voice = SPU_VOICECH (VOICE_CH);
    s_attr.mask = SPU_VOICE_PITCH;
    s_attr.pitch = (unsigned short) value;

    SpuSetKeyOnWithAttr (&s_attr);
}

/* Normal Voice pitch sweep up/down */
void
keyAttr (int value)
{
    SpuVoiceAttr s_attr;

    s_attr.voice = SPU_VOICECH (VOICE_CH);
    s_attr.mask  = SPU_VOICE_PITCH;
    s_attr.pitch = (unsigned short) value;

    SpuSetVoiceAttr (&s_attr);
}

/* Normal Voice Key off */
void
keyOff (void)
{
    PRINTF (("Normal voice : Key Off\n"));
    SpuSetKey (SpuOff, SPU_VOICECH(VOICE_CH));
}


/* Normal Voice Key on */
void
noiseOn (int value)
{
#ifdef DEBUG
    if (value == 0 || value == 0x3f) {
	PRINTF (("Noise voice : Key On\n"));
    }
#endif /* DEBUG */

    SpuSetNoiseClock (value);

    SpuSetKey (SpuOn, SPU_VOICECH (NOISE_CH));
}

/* Noise Voice pitch sweep up */
void
noiseAttr (int value)
{
    SpuSetNoiseClock (value);
}

/* Noise Voice Key off */
void
noiseOff (void)
{
    PRINTF (("Noise voice : Key Off\n"));
    SpuSetKey (SpuOff, SPU_VOICECH (NOISE_CH));
}

#define NOISE_INTERVAL 2
#define PITCH_INC 0x20
#define PITCH_DEC 0x20

void
padHandle (unsigned long padd)
{
    static int key_Rup    = False;
    static int key_Rdown  = False;
    static int key_Lup    = False;
    static int key_Ldown  = False;

    static int vp = 0;
    static int np;
    static int ni;

    /* Pitch Up */
    if (padd & PADLup) {
	if ((key_Lup == False)) {
	    vp = 0;
	    keyOn (vp);
	    key_Lup = True;
	} else {
	    /* butten is held */
	    if (vp < (0x4000 - PITCH_INC)) {
		vp += PITCH_INC;
		keyAttr (vp);
	    }
	}
    } else {
	if ((key_Lup == True)) {
	    keyOff ();
	    key_Lup = False;
	}
    }

    /* Pitch Down */
    if (padd & PADLdown) {
	if ((key_Ldown == False)) {
	    vp = 0x3fff;
	    keyOn (vp);
	    key_Ldown = True;
	} else {
	    /* butten is held */
	    if (vp >= PITCH_DEC) {
		vp -= PITCH_DEC;
		keyAttr (vp);
	    }
	}
    } else {
	if ((key_Ldown == True)) {
	    keyOff ();
	    key_Ldown = False;
	}
    }

    /* Noise Up */
    if (padd & PADRup) {
	if ((key_Rup == False)) {
	    np = 0;
	    noiseOn (np);
	    key_Rup = True;
	    ni = 0;
	} else {
	    /* butten is held */
	    if (ni < NOISE_INTERVAL) {
		ni ++;
	    } else {
		ni = 0;
		if (np < 0x40) {
		    np ++;
		    noiseAttr (np);
		}
	    }
	}
    } else {
	if ((key_Rup == True)) {
	    noiseOff ();
	    key_Rup = False;
	}
    }

    /* Noise Down */
    if (padd & PADRdown) {
	if ((key_Rdown == False)) {
	    np = 0x3f;
	    noiseOn (np);
	    key_Rdown = True;
	    ni = 0;
	} else {
	    /* butten is held */
	    if (ni < NOISE_INTERVAL) {
		ni ++;
	    } else {
		ni = 0;
		if (np >= 0) {
		    np --;
		    noiseAttr (np);
		}
	    }
	}
    } else {
	if ((key_Rdown == True)) {
	    noiseOff ();
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
