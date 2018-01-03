/* $PSLibId: Run-time Library Release 4.4$ */
#ifndef _MENUSET_H_
#define _MENUSET_H_

/*****************************************************************
 *
 * file: menuset.h
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
/*
   Parts List for Menu System "menuset.h"
*/
#include "maxconf.h"

#include "pad.h"

#ifdef ENGLISH
#include "memeng.h"
#else /* ENGLISH */
#include "memjpn.h"
#endif /* ENGLISH */

#include "menuset2.h"

_RMENU	RM[] = {
	{ 0,  1, NULL,   107,  50+35, 172, 50, 0, 2, 0, 0, Blink, NULL, NULL	, 0 },
	{ 0,  2, NULL,   533,  50+35, 172, 50, 1, 0, 0, 0, Blink, NULL, NULL	, 0 },
	{ 2,  3, NULL, 320, 138, 172, 50,  0,  0, 6, 4, Blink, NULL, funcExit, OKKey|_PADLup|_PADLdown},
	{ 2,  4, NULL, 320, 209, 172, 50, 30, 60, 3, 5, Blink, NULL, funcMenu, OKKey|CDKey|_PADLright|_PADLleft|_PADLup|_PADLdown },
	{ 2,  5, NULL, 320, 280, 172, 50, 30, 60, 4, 6, Blink, NULL, funcMenu, OKKey|CDKey|_PADLright|_PADLleft|_PADLup|_PADLdown },
	{ 2,  6, NULL, 320, 351, 172, 50, 30, 60, 5, 3, Blink, NULL, funcMenu, OKKey|CDKey|_PADLright|_PADLleft|_PADLup|_PADLdown },
	{100, 10,NULL, 273, 266,60, 32, 0,11, 0, 0, Blink, NULL, funcMemConf, OKKey },
	{100, 11,NULL, 367, 266,60, 32,10, 0, 0, 0, Blink, NULL, funcMemConf, OKKey },
#ifdef ENGLISH
	{100, 12, NULL,  280, 200,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
	{100, 13, NULL,  280, 200,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
#else
	{100, 12, NULL,  339, 195,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
	{100, 13, NULL,  339, 195,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
#endif /* ENGLISH */
	{  0, 14, NULL, XK+KW/4, YK+KH/2,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
	{  0, 15, NULL, XK+KW*3/4, YK+KH/2,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
#ifdef ENGLISH
	{100, 17, NULL, 320, 228,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
	{100, 18, NULL, 370, 200,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
	{100, 19, NULL, 365, 200,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
#else
	{100, 17, NULL, 320, 208,0, 0, 0, 0, 0, 0, 0, NULL, NULL, OKKey },
#endif /* ENGLISH */
	{ 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1, NULL, NULL, OKKey }
};
/*---------------------------------------------------------------------------*/
_INIT	InitList[] = {
	{ (u_long)_TITLE, 320,   50+35,          5, -1, 0,  0,  0, -1 },
	{  _COW0 ,  107,         50+35,          5, -1, 1,172, 56-6, -1 },
	{  _COW1 ,  533,         50+35,          5, -1, 1,172, 56-6, -1 },
 	{  _COW2 ,  320,         138,            5, -1, 1,172, 56-6, -1 },
	{  _COW3 ,  320,         209,            5, -1, 1,172, 56-6, -1 },
	{  _COW4 ,  320,         280,            5, -1, 1,172, 56-6, -1 },
	{  _COW5 ,  320,         351,            5, -1, 1,172, 56-6, -1 },
	{  _SPH2 ,  273,         266,     HIDE, -1, 1, 40, 40, 110 },  /**_SPH2-7**/
	{     -1 ,  367,         266,     HIDE,  7, 1, 40, 40, 111 },  /* _SPH2 */
	{    NULL,  0,  0, 0, 0 }
};
/*---------------------------------------------------------------------------*/
/*      {   SPRT   x,            y,   priority,copy,effect,
                                                     width,height,useMenu }  */
           
_INIT   BGSprtList[] = {
/* Area Dots */
	{  _SPH2 , XC1,          YC,         BP, -1, 1, BS, BS, -1 },  /**_SPH2-0**/
	{     -1 , XC1+DC,       YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1,          YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC,       YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1,          YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC,       YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1,          YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC,       YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1,          YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC,       YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1,          YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC,       YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*2,     YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC1+DC*3,     YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC,         BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC+DC,      BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC+DC*2,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC+DC*3,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC+DC*4,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2,          YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC,       YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*2,     YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
	{     -1 , XC2+DC*3,     YC+DC*5,    BP,  0, 0, BS, BS, -1 },  /* _SPH2 */
/* Kanji Plate Area Dots */
	{     -1 , XK,           YK,         BP,  0, 0,MBS,MBS, -1 },  /* _SPH2 */
	{     -1 , XK+KW,        YK,         BP,  0, 0,MBS,MBS, -1 },  /* _SPH2 */
	{     -1 , XK,           YK+KH,      BP,  0, 0,MBS,MBS, -1 },  /* _SPH2 */
	{     -1 , XK+KW,        YK+KH,      BP,  0, 0,MBS,MBS, -1 },  /* _SPH2 */
/* Kanji Plate */
	{  _SPH8 , XK+KW/2,      YK+KH/2,     6, -1, 2, KW, KH, -1 },  /**_SPH8-52**/
/* Confirm Window */
	{     -1 , 320,          240,         HIDE, 52, 1,260,120, -1 },  /* _SPH8 */
	{    NULL,   0,   0, 0, 0 }
};

/*---------------------------------------------------------------------------*/
_INIT   Cursor[] = {
	{ (u_long)pad_cursor , 100, 100, 1, -1,  1,  42,  32, -1 },
	{    NULL,   0,   0, 0, 0 }
};
/*---------------------------------------------------------------------------*/
_IMAGEMAP   IM[] = {
    {   0,   0,  88,  24,   0,   0 },  /* card1 */
    {  88,   0,  88,  24,  22,   0 },  /* card2 */

    {   0,  24,  58,  19,   0,  24 },  /* exit */
    {  60,  24,  70,  19,  15,  24 },  /* copy */
    { 128,  24, 120,  19,  32,  24 },  /* copy all */
 
    {   0,  43,  88,  19,   0,  43 },  /* delete */
    {  88,  43,  40,  16,  22,  43 },  /* yes */
    { 128,  43,  40,  16,  32,  43 },  /* no */

    {  56,  24,  72,  19,  14,  24 },  /* copy */
    {   0,  43,  88,  19,   0,  43 },  /* delete */

    { 168,  43,  80,  13,  42,  43 },  /* blank */
    { 168,  43,  80,  13,  42,  43 },  /* blank */

#ifdef ENGLISH
    {   0, 104, 168,  26,   0, 104 },  /* confirm message */
    { 176,   0,  48,  19,  44,   0 },  /* confirm file    */
    {   0,   5,  72,  19,   0,   5 }   /* confirm card    */
#else
    {   0,  62, 236,  42,   0,  62 }   /* confirm message */
#endif /* ENGLISH */
};

_PAGERES	Page[] = {
	{ NULL, NULL, NULL, NULL, NULL, {0, 0, 640, 480}, {0, 480, 832, 32}, NULL},
	{ RM, BGSprtList, InitList, Cursor, 3, {0, 0, 640, 480}, {0, 480, 640, 32}, IM},
	{ NULL, NULL, NULL, NULL, NULL, {0, 0, 640, 480}, {0, 480, 832, 32}, NULL},
};
/*---------------------------------------------------------------------------*/
#endif /* _MENUSET_H_ */
/* DON'T ADD STUFF AFTER THIS */
