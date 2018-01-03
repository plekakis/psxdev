/************************************************************
 *                                                          *
 *                        main.h                            *
 *                                                          *
 *                                                          *
 *                Vince Diesi     13/02/97                  *
 *                                                          *
 *   Copyright (C) 1997 Sony Computer Entertainment Inc.    *
 *                  All Rights Reserved                     *
 *                                                          *
 ***********************************************************/

#ifndef __MAIN_H
#define __MAIN_H

/* ---------------------------------------------------------------------------
 * - CONSTANTS
 * ---------------------------------------------------------------------------
 */

// Screen position and dimensions. 
#define	FRAME_X			512

#ifdef NTSC

#define	FRAME_Y			240
#define SCREEN_X		0
#define SCREEN_Y		0	

#else

#define	FRAME_Y			256
#define SCREEN_X		0
#define SCREEN_Y		18	

#endif // NTSC

/* ---------------------------------------------------------------------------
 * - DATA TYPE AND STRUCTURE DECLARATIONS
 * ---------------------------------------------------------------------------
 */
 
typedef struct {
	DRAWENV		draw;
	DISPENV		disp;
} DB;

/* ---------------------------------------------------------------------------
 * - GLOBAL DECLARATIONS
 * ---------------------------------------------------------------------------
 */
 
extern DB		db[2];		
extern short	cdb;
extern long		fIdA;				// Aplic font id.

/* ---------------------------------------------------------------------------
 * - LIBSN.LIB GLOBAL DECLARATIONS
 * ---------------------------------------------------------------------------
 */

extern unsigned long __heapbase;
extern unsigned long __heapsize;
extern unsigned long __bss;
extern unsigned long __bsslen;
extern unsigned long __data;
extern unsigned long __datalen;
extern unsigned long __text;
extern unsigned long __textlen;
extern unsigned long _ramsize;
extern unsigned long _stacksize;

/* ------------------------------------------------------------------------ */

#endif // __MAIN_H 
