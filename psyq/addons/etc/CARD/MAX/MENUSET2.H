/* $PSLibId: Runtime Library Release 3.6$ */
#ifndef _MENUSET2_H_
#define _MENUSET2_H_

/*****************************************************************
 *
 * file: menuset2.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

/* for Memory Card */
#define	_ID_NOTHING	0
#define	_ID_CARD_A	1
#define	_ID_CARD_B	2
#define	_ID_EXIT	3
#define	_ID_COPY	4
#define	_ID_ACOPY	5
#define	_ID_DELETE	6
#define	_ID_MOVE	7

#define	_ID_ICON_A	30
#define	_ID_ICON_B	60

#define _CONF_ID_YES    10
#define _CONF_ID_NO     11
#define _CONF_ID_COPY   12
#define _CONF_ID_DELETE 13
#define _CONF_ID_MESSAGE 17
#define _CONF_ID_FILE    18
#define _CONF_ID_CARD    19

#define _CONF_SPRT_BALL_YES  110
#define _CONF_SPRT_BALL_NO   111
#define _CONF_BG_PLATE       53

#ifdef EMOUSE

/*カードのメモリーアイコン位置 */
#define _ID_COPY_MEM     24
#define _ID_DELETE_MEM   26
/*サイズ */
#define	BLUE_DX	(DELTA-16)
#define	BLUE_DY	(DELTA-16)

/*バックグラウンドパーツｉｄ */
#define _ID_COPY_BLUE    14
#define _ID_DELETE_BLUE  16

#endif /* EMOUSE */

/* call backs */
/* MEMORY_CARD */
static void     funcMemCB     ( long pad );
void            funcMemExit   ( long pad );
static void	funcMemConf   ( long pad );
extern void     funcExit( long pad );
/*-----*/
static void     funcDummy   ( long pad );

/* page setup infomation */
typedef struct {
    short x, y;
    short w, h;
    short u, v;
} _IMAGEMAP;

typedef struct {
	_RMENU	*menu;
	_INIT	*bg;
	_INIT	*sprt;
	_INIT   *cursor;
	int	currentItem;
	RECT	maskRect;
	RECT	clutRect;
	_IMAGEMAP  *imap;
} _PAGERES;

/*---------------------------------------------------------------------------*/
#define XC1 32        /* Memory Card-1 Icons' Dots origin center x */
#define XC2 (XC1+384+42) /* Memory Card-2 Icons' Dots origin center x */
#define YC  (90+35)     /* Memory Card   Icons' Dots origin center y */
#define DC  (64-14)     /* Memory Card Icons Sizes (w, h) */
#define XK  XC1       /* Kanji Plate origin x */
#define YK  (YC+340-70)  /* Kanji Plate origin y */
#define KW  576       /* Kanji Plate Width */
#define KH  32        /* Kanji Plate Height */
#define BS   8        /* Dots Size */
#define MBS 16        /* Kanji Plate Balls Size */
#define BP   5        /* Area Balls Priority */

#define HIDE OTSIZE-2 

#define Blink 0x01

#endif /* _MENUSET2_H_ */
/* DON'T ADD STUFF AFTER THIS */
