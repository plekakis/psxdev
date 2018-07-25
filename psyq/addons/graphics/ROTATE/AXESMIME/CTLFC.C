/* $PSLibId: Runtime Library Release 3.6$ */
/***********************************************
 *	ctlfc.c
 *
 *	Copyright (C) 1996 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
/* 動きを決める伝達特性のテーブル */
#include "ctlfc.h"
int  cnv0[CNVLEN] = {
	  20,  30,  40,  50,  59,  79,  98, 128,
	 157, 177, 197, 206, 216, 226, 236, 238,
	 236, 226, 216, 206, 197, 177, 157, 128,
	  98,  79,  59,  50,  40,  30,  20,  10
};
