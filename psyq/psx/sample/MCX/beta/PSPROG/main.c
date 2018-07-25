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

	/* port number*/
	chan = 0x00;

#if 0
	/* card connection test*/
	MemCardAccept(chan);

	/* wait for MemCardAccept to finish*/
	MemCardSync(0,0,&rslt);

	/* error handling*/
	if(rslt!=McErrNone && rslt!=McErrNewCard) {
		rprint(rslt);
		return;
	}
#endif

	/* format*/
	printf("format\n");
	rslt =MemCardFormat( chan );
	if( rslt!=McErrNone ) {
		rprint(rslt);
		return;
	}

	printf("create program 1\n");
	/* file creation 1 block*/
	rslt =MemCardCreateFile( chan, "BISLPSP99990", 1 );
	if( rslt!=McErrNone ) {
		rprint(rslt);
		return;
	}
	/* write file data  1 block's worth*/
	printf("write program 1\n");
	MemCardWriteFile( chan, "BISLPSP99990", (long*)0x80100000, 0, 8192);
	/* wait for completion*/
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

	/* initialize memory card system*/
	MemCardInit(1);
	MemCardStart();

	makecard();

	/* stop memory card system
*/
	MemCardStop();
	MemCardEnd();
	PadStop();
	StopCallback();
	return 0;
}

