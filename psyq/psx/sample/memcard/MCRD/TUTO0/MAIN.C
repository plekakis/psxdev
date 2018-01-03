/*
 * $PSLibId: Run-time Library Release 4.3$
 */
/*
 *	Memory Card Sample Program  - TUTO0 -
 *
 *
 *	Copyright (C) 1997 by Sony Computer Entertainment
 *			All rights Reserved
 *
*/
#include <stdio.h>
#include <sys/types.h>
#include <libapi.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>
#include <libmcrd.h>
#include "hand1.h"
#include "hand2.h"
#include "hand3.h"


/* 4-bit TIM header structure */
typedef	struct {
    long	id;
    long	flag;
    long	cbnum;
    short	cx;
    short	cy;
    short	cw;
    short	ch;
    char	clut[32];
    long	pbnum;
    short	px;
    short	py;
    short	pw;
    short	ph;
    char	image[2];
} _TIM4;

/* file header structure */
typedef struct {
	char	Magic[2];
	char	Type;
	char	BlockEntry;
	char	Title[64];
	char	reserve[28];
	char	Clut[32];
	char	Icon[3][128];
} _CARD;

static _CARD HEAD;
/***************************************************************************

***************************************************************************/
void rprint( long rslt )
{
	switch(rslt) {
		case McErrNone:
		break;
		case McErrCardNotExist:
		printf( "card is not ready\n" );
		break;
		case McErrCardInvalid:
		printf( "card is invalid\n" );
		break;
		case McErrNewCard:
		printf( "card is changed\n" );
		break;
		case McErrNotFormat:
		printf( "not format\n" );
		break;
		case McErrFileNotExist:
		printf( "file not exist\n" );
		break;
		case McErrAlreadyExist:
		printf( "file already exist\n" );
		break;
		case McErrBlockFull:
		printf( "free block empty\n" );
		break;
		default:
		printf( "error(%d)\n",(int)rslt );
		break;
	}
}
void makecard(void)
{
	long chan;
	unsigned long rslt;

	printf( "Make Card Sample program\n" );

	/* port number */
	chan = 0x00;

#if 0
	/* test for card connection */
	MemCardAccept(chan);

	/* wait for MemCardAccept to finish */
	MemCardSync(0,0,&rslt);

	/* error handling */
	if(rslt!=McErrNone && rslt!=McErrNewCard) {
		rprint(rslt);
		return;
	}
#endif

	/* create file */
	rslt =MemCardCreateFile( chan, "HAND", 1 );
	if( rslt!=McErrNone ) {
		rprint(rslt);
		if(rslt==McErrAlreadyExist)
			MemCardDeleteFile(chan, "HAND" );
		return;
	}

	/* prepare file header */
	HEAD.Magic[0] = 'S';
	HEAD.Magic[1] = 'C';
	HEAD.Type = 0x13;
	HEAD.BlockEntry = 1;
	strcpy( HEAD.Title, "じゃんけん　ぽん　グーチョキパー" );
	memcpy( HEAD.Clut, ((_TIM4*)hand1)->clut, 32 );
	memcpy( HEAD.Icon[0], ((_TIM4*)hand1)->image, 128);
	memcpy( HEAD.Icon[1], ((_TIM4*)hand2)->image, 128);
	memcpy( HEAD.Icon[2], ((_TIM4*)hand3)->image, 128);

	/* write file data */
	MemCardWriteFile( chan, "HAND", (long*)&HEAD, 0, sizeof(HEAD));

	/* wait for finish */
	MemCardSync(0,0,&rslt);

	if(rslt!=McErrNone) {
		rprint(rslt);
		return;
	}
	printf( "done.\n" );
}

int main(void)
{
	ResetCallback();
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	PadInit(0);

	/* initialize memory card system */
	MemCardInit(1);
	MemCardStart();

	makecard();

	/* stop memory card system */
	MemCardStop();
	MemCardEnd();
	PadStop();
	StopCallback();
	return 0;
}
