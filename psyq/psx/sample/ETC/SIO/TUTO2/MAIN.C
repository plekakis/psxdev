/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * 	sio echo back 
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
static void init_sio();

main()
{
	ResetCallback();
	init_sio();
	Sio1Callback(sio_read);
	ResetGraph(0);
	PadInit(0);
	while(1)
		if(PadRead(1)&PADselect)break;
	PadStop();
	ResetGraph(3);
	StopCallback();
	return;
}

static void init_sio()
{
	_sio_control(1,1,CR_RXIEN|CR_RXEN|CR_TXEN|CR_RTS|CR_DTR);
	_sio_control(1,2,MR_SB_11|MR_CHLEN_8|MR_BR_16);
	_sio_control(1,3,9600);
	printf("%x\n",_sio_control(0,0,0));
	printf("%x\n",_sio_control(0,1,0));
	printf("%x\n",_sio_control(0,2,0));
	printf("%d\n",_sio_control(0,3,0));
}


static void sio_read()
{
	char c;

	_sio_control(2,1,0);
	if(_sio_control(0,0,0)&SR_RXRDY)
	{
		c=_sio_control(0,4,0);
		if(_sio_control(0,0,0)&SR_TXRDY)
			_sio_control(1,4,c);
	}
}

