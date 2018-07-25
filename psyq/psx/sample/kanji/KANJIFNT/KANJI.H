/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#ifndef _KANJI_H_
#define _KANJI_H_

/*****************************************************************
 *
 * file: kanji.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#if 0
#define DOT11	/* 11 dot font */
#define DOT13	/* 13 dot font */
#define DOT15	/* 15 dot font */
#endif

#define DOT15	/* 15 dot font */

#ifdef DOT11
#define DOT		11
#define KANJIADDR0	got11j0b
#define KANJIADDR1	got11j1b
#endif /* DOT11 */

#ifdef DOT13
#define DOT		13
#define KANJIADDR0	got13j0b
#define KANJIADDR1	got13j1b
#endif /* DOT13 */

#ifdef DOT15
#define DOT		15
#define KANJIADDR0	got15j0b
#define KANJIADDR1	got15j1b
#define KANJIADDRG	got15gai
#endif /* DOT15 */

#endif /* _KANJI_H_ */
/* DON'T ADD STUFF AFTER THIS */
