/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*****************************************************************
 *
 * file: trans.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include "kanji.h"
#include	"../fontdata/offset.h"

#if (DOT==11)
#include	"../fontdata/got11j0b.h"
#include	"../fontdata/got11j1b.h"
#endif
#if (DOT==13)
#include	"../fontdata/got13j0b.h"
#include	"../fontdata/got13j1b.h"
#endif
#if (DOT==15)
#include	"../fontdata/got15j0b.h"
#include	"../fontdata/got15j1b.h"
#include	"../fontdata/got15gai.h"
#endif

long kanji_test( unsigned char *sjis );
int kanjitrans(unsigned short scode);

long kanji_test( unsigned char *sjis )
{
  
  int	i,code;
  unsigned long	kaddr, addr;
  unsigned short sjiscode;

  sjiscode = *sjis << 8 | *(sjis+1);
  if ((sjiscode >= 0x8140) && (sjiscode <= 0x84be))
    kaddr = (unsigned long)KANJIADDR0;		/* KANJIADDR0 */
#if (DOT==15)
  else
    if ((sjiscode >= 0x8540) && (sjiscode <= 0x8796))
      kaddr = (unsigned long)KANJIADDRG;	/* KANJIADDRG */
#endif
  else
    if ((sjiscode >= 0x889f) && (sjiscode <= 0x9872))
      kaddr = (unsigned long)KANJIADDR1; 	/* KANJIADDR1 */
    else {
      printf("bad sjis code 0x%x\n", sjiscode);
    }
  
  code = kanjitrans(sjiscode);
  addr = kaddr + code * DOT * 2;

  return addr;
}

/* 
 * sjis code to access number transfer function 
 */
int kanjitrans(unsigned short scode)
{
	int ret = 0;
	unsigned char ss[2];
	unsigned char stmp;

	bcopy(&scode, ss, 2);
	
	switch(ss[1]) {
	case 0x81:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x7e))
			stmp = 0;
		else
		if ((ss[0] >= 0x80) && (ss[0] <= 0xac))
			stmp = 1;
		else
		if ((ss[0] >= 0xb8) && (ss[0] <= 0xbf))
			stmp = 2;
		else
		if ((ss[0] >= 0xc8) && (ss[0] <= 0xce))
			stmp = 3;
		else
		if ((ss[0] >= 0xda) && (ss[0] <= 0xe8))
			stmp = 4;
		else
		if ((ss[0] >= 0xf0) && (ss[0] <= 0xf7))
			stmp = 5;
		else
		if (ss[0] == 0xfc)
			stmp = 6;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_0_table[stmp][0] + kanji_0_table[stmp][1];
		break;

	case 0x82:
		if ((ss[0] >= 0x4f) && (ss[0] <= 0x58))
			stmp = 7;
		else
		if ((ss[0] >= 0x60) && (ss[0] <= 0x79))
			stmp = 8;
		else
		if ((ss[0] >= 0x81) && (ss[0] <= 0x9a))
			stmp = 9;
		else
		if ((ss[0] >= 0x9f) && (ss[0] <= 0xf1))
			stmp = 10;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_0_table[stmp][0] + kanji_0_table[stmp][1];
		break;

	case 0x83:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x7e))
			stmp = 11;
		else
		if ((ss[0] >= 0x80) && (ss[0] <= 0x96))
			stmp = 12;
		else
		if ((ss[0] >= 0x9f) && (ss[0] <= 0xb6))
			stmp = 13;
		else
		if ((ss[0] >= 0xbf) && (ss[0] <= 0xd6))
			stmp = 14;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_0_table[stmp][0] + kanji_0_table[stmp][1];
		break;

	case 0x84:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x60))
			stmp = 15;
		else
		if ((ss[0] >= 0x70) && (ss[0] <= 0x7e))
			stmp = 16;
		else
		if ((ss[0] >= 0x80) && (ss[0] <= 0x91))
			stmp = 17;
		else
		if ((ss[0] >= 0x9f) && (ss[0] <= 0xbe))
			stmp = 18;
		ret = scode - kanji_0_table[stmp][0] + kanji_0_table[stmp][1];
		break;

	case 0x85:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x44))
			stmp = 0;
		else
		if (ss[0] == 0x46)
			stmp = 1;
		else
		if ((ss[0] >= 0x48) && (ss[0] <= 0x49))
			stmp = 2;
		else
		if (ss[0] == 0x4b)
			stmp = 3;
		else
		if ((ss[0] >= 0x50) && (ss[0] <= 0x5b))
			stmp = 4;
		else
		if ((ss[0] >= 0xa0) && (ss[0] <= 0xac))
			stmp = 5;
		else
		if ((ss[0] >= 0xb0) && (ss[0] <= 0xbc))
			stmp = 6;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_g_table[stmp][0] + kanji_g_table[stmp][1];
		break;

	case 0x86:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x41))
			stmp = 7;
		else
		if (ss[0] == 0x43)
			stmp = 8;
		else
		if (ss[0] == 0x45)
			stmp = 9;
		else
		if ((ss[0] >= 0x47) && (ss[0] <= 0x48))
			stmp = 10;
		else
		if ((ss[0] >= 0x4a) && (ss[0] <= 0x4c))
			stmp = 11;
		else
		if ((ss[0] >= 0x4e) && (ss[0] <= 0x4f))
			stmp = 12;
		else
		if ((ss[0] >= 0x60) && (ss[0] <= 0x6f))
			stmp = 13;
		else
		if ((ss[0] >= 0x9f) && (ss[0] <= 0xb6))
			stmp = 14;
		else
		if ((ss[0] >= 0xb8) && (ss[0] <= 0xf5))
			stmp = 15;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_g_table[stmp][0] + kanji_g_table[stmp][1];
		break;

	case 0x87:
		if ((ss[0] >= 0x40) && (ss[0] <= 0x5a))
			stmp = 16;
		else
		if ((ss[0] >= 0x5c) && (ss[0] <= 0x62))
			stmp = 17;
		else
		if ((ss[0] >= 0x64) && (ss[0] <= 0x96))
			stmp = 18;
		else {
			ret = -1;
			break;
		}
		ret = scode - kanji_g_table[stmp][0] + kanji_g_table[stmp][1];
		break;

	default:
		if ((ss[1] > 0x98) || ((ss[1] == 0x98) && (ss[0] > 0x72)) ||
		    (ss[1] < 0x88) || ((ss[1] == 0x88) && (ss[0] < 0x9f)) ||
		    (ss[0] > 0xfc) || (ss[0] < 0x40) ||
		    ((ss[0] > 0x7e) && (ss[0] < 0x80))) {
			ret = -1;
			break;
		}
		stmp = ((ss[1] - 0x88)*2) + ((ss[0] < 0x7f) ? 0 : 1) - 1;
		ret = scode - kanji_1_table[stmp][0] + kanji_1_table[stmp][1];
	}
	
#ifdef DEBUG
	printf("scode=0x%x, ss0=0x%x, ss1=0x%x, ret=0x%x, stmp=0x%x\n", 
		scode, ss[0], ss[1], ret, stmp);
#endif /* DEBUG */
	return(ret);
}
