/*****************************************************************
 * -*- c -*-
 * $RCSfile: tuto5.c,v $
 *
 * Copyright (C) 1994 by Sony Computer Entertainment Inc.
 * All Rights Reserved.
 *
 * Sony Computer Entertainment Inc. R & D Division
 *
 *****************************************************************/
/*
 * $PSLibId: Run-time Library Release 4.4$
 */

#ifndef lint
static char rcsid [] = "$Id: tuto5.c,v 1.8 1997/08/29 07:35:09 tetsu Exp $ : \
	Copyright (C) by 1994 Sony Computer Entertainment Inc.";
#endif


#include <r3000.h>
#include <asm.h>
#include <libapi.h>

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libspu.h> 
#ifdef __psx__
#include <libsn.h>
#else
#define pollhost()
#endif

#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif

int   end_flag;

/* ================================================================
 *    Main	
 * ================================================================ */

#define MALLOC_MAX 2
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

#ifdef OCT_13TH
#define PF_DATA_SIZE 38928
unsigned char pf_wave [] = {
#include "pf.h"
};
#else
#define PF_DATA_SIZE 18704
unsigned char pf_wave [] = {
#include "pf_wave.h"
};
#endif /* OCT_13TH */

#define DATA_SIZE PF_DATA_SIZE

#define BUFSIZE 2048
unsigned char buf [BUFSIZE];

/* #define USE_WITHATTR /**/

main (void)
{
    DRAWENV draw;		/* drawing environment */
    DISPENV disp;		/* display environment */

    int w1, w2;

    long size, size_;

    long addr1, addr2, a_addr;
    SpuVoiceAttr s_attr, s_attr2;
    SpuCommonAttr c_attr;

    /* ----------------------------------------------------------------
     *		Initialize interrupt environment. */

    ResetCallback();

    /* ----------------------------------------------------------------
     *		Initialize Graphics system */

    ResetGraph (0);

    /* ----------------------------------------------------------------
     *		Initialize SPU */

    SpuInit ();
    SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);

    /* ----------------------------------------------------------------
     *		Initialize Control pad */

    PadInit (0);

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
     *		Set common attributes */

    c_attr.mask = (SPU_COMMON_MVOLL |
		   SPU_COMMON_MVOLR);

    c_attr.mvol.left  = 0x3fff;	/* Master volume (left) */
    c_attr.mvol.right = 0x3fff;	/* Master volume (right) */

    SpuSetCommonAttr (&c_attr);

    /* ----------------------------------------------------------------
     *		Set waveform transfer */

    SpuSetTransferMode (SpuTransByDMA); /* transfer by DMA */

    /* Area transferred first */
    if ((addr1 = SpuMalloc (DATA_SIZE)) < 0) {
	printf ("There is NO more enough memory (size : %d)\n",
		DATA_SIZE);
	return;
    }

    /* Area transferred later */
    if ((addr2 = SpuMalloc (DATA_SIZE)) < 0) {
	printf ("There is NO more enough memory (size : %d)\n",
		DATA_SIZE);
	return;
    }

    a_addr = addr1;
    SpuSetTransferStartAddr (a_addr);

    /* ----------------------------------------------------------------
     *		Set voice attributes */

    /* Voice attributes */
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

    /* Piano sound transferred first */
    s_attr.voice = (SPU_1CH);

    s_attr.volume.left  = 0x1fff;
    s_attr.volume.right = 0x1fff;
    s_attr.pitch        = 0x800;
    s_attr.addr         = addr1;
    s_attr.a_mode       = SPU_VOICE_LINEARIncN;
    s_attr.s_mode       = SPU_VOICE_LINEARIncN;
    s_attr.r_mode       = SPU_VOICE_LINEARDecN;
    s_attr.ar           = 0x0;
    s_attr.dr           = 0x0;
    s_attr.sr           = 0x0;
    s_attr.rr           = 0x3;
    s_attr.sl           = 0xf;

#ifdef USE_WITHATTR
    s_attr2.mask = s_attr.mask;

    /* Piano sound transferred later */
    s_attr2.voice = (SPU_2CH);

    s_attr2.volume.left  = s_attr.volume.left;
    s_attr2.volume.right = s_attr2.volume.right;
    s_attr2.pitch        = 0x1000;
    s_attr2.addr         = addr2;
    s_attr2.a_mode       = s_attr.a_mode;
    s_attr2.a_mode       = s_attr.a_mode;
    s_attr2.s_mode       = s_attr.s_mode;
    s_attr2.r_mode       = s_attr.r_mode;
    s_attr2.ar           = s_attr.ar;
    s_attr2.dr           = s_attr.dr;
    s_attr2.sr           = s_attr.sr;
    s_attr2.rr           = s_attr.rr;
    s_attr2.sl           = s_attr.sl;
#else
    SpuSetVoiceAttr (&s_attr);

    /* Piano sound transferred later */

    s_attr.voice = (SPU_2CH);
    s_attr.pitch        = 0x1000;
    s_attr.addr         = addr2;

    SpuSetVoiceAttr (&s_attr);
#endif /* USE_WITHATTR */

    /* ----------------------------------------------------------------
     *		Transfer & Key ON */

    size  = DATA_SIZE;
    size_ = 0;
    end_flag = 0;

    w1 = w2 = 0;
    while (! end_flag) {

	FntPrint ("\n\n\n\n\tLibspu test program: test5.c\n");

	memcpy (buf, &pf_wave [size_], BUFSIZE);

	SpuWritePartly (buf, BUFSIZE);
	while (! SpuIsTransferCompleted (SPU_TRANSFER_GLANCE)) {
	    /* printf ("."); /**/
	}
	/* printf ("\n"); /**/

	size_ += BUFSIZE;
	size  -= BUFSIZE;

	if (size <= 0) {

	    size  = DATA_SIZE;
	    size_ = 0;

	    if (a_addr == addr1) {
		w1 = 1;
		w2 = 0;
#ifdef USE_WITHATTR
		SpuSetKeyOnWithAttr (&s_attr);
#else
		SpuSetKey (SpuOn,  SPU_1CH);
#endif /* USE_WITHATTR */
		SpuSetKey (SpuOff, SPU_2CH);

		/* Next transfer address */
		a_addr = addr2;

	    } else {
		w1 = 0;
		w2 = 1;
#ifdef USE_WITHATTR
		SpuSetKeyOnWithAttr (&s_attr2);
#else
		SpuSetKey (SpuOn,  SPU_2CH);
#endif /* USE_WITHATTR */
		SpuSetKey (SpuOff, SPU_1CH);

		/* Next transfer address */
		a_addr = addr1;
	    }

	    SpuSetTransferStartAddr (a_addr);
	}

	FntPrint ("\n\n");

	FntPrint ("\t        : Key On / Status\n");
	FntPrint ("\tVoice 1 : %d        %s\n",
		  w1,
		  (SpuGetKeyStatus (SPU_1CH) == SpuOn)    ? "ON" :
		  (SpuGetKeyStatus (SPU_1CH) == SpuOff)   ? "OFF" :
		  (SpuGetKeyStatus (SPU_1CH) == SpuOffEnvOn) ? "Env ON" : "Env OFF"
		  );
	FntPrint ("\tVoice 2 : %d        %s\n",
		  w2,
		  (SpuGetKeyStatus (SPU_2CH) == SpuOn)    ? "ON" :
		  (SpuGetKeyStatus (SPU_2CH) == SpuOff)   ? "OFF" :
		  (SpuGetKeyStatus (SPU_2CH) == SpuOffEnvOn) ? "Env ON" : "Env OFF"
		  );
	
        if (PadRead (0) & PADk)
	    end_flag = 1;
#if 0
	pollhost ();
#endif
	VSync (0);
	ClearImage ((RECT*)&disp, 0, 0, 0 );
	FntFlush (-1);
    }

    /* ----------------------------------------------------------------
     *		Finalize this sample */

    VSync (0);
    ClearImage ((RECT*)&disp, 0, 0, 0);
    FntFlush (-1);

    SpuSetKey (SPU_OFF, SPU_ALLCH);

    SpuQuit ();

    PadStop ();		/* pad stop */

    ResetGraph (3);
    StopCallback ();

    return;
}

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/* DON'T ADD STUFF AFTER THIS */
