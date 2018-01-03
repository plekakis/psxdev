/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	sio echo back 
 *
 *	Copyright (C) 1997 by Sony Corporation
 *		All rights Reserved
 *
 * 	Version		Date		Design
 *	---------------------------------------
 *	1.00		Jan.28.1997	shino
 */

#include <sys/types.h>
#include <libetc.h>
#include <libsio.h>
static void sio_read();

main()
{
	ResetCallback();
	AddSIO(9600);
	ResetGraph(0);
	PadInit(0);
	while(1)
	{
		if(PadRead(1)&PADselect)break;
		sio_read();
	}
	DelSIO();
	PadStop();
	ResetGraph(3);
	StopCallback();
	return;
}



static void sio_read()
{
	char c;

	c=getchar();
	putchar(c);
}

