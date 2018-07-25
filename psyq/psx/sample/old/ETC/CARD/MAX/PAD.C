/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: pad.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "libps.h"

#include "maxtypes.h"
#include "maxstat.h"
#include "maxproto.h"

extern u_long	padd, opadd;

int pad_read(void)
{

#ifndef EMOUSE
	opadd = padd;
	padd = PadRead(0);
#endif /* EMOUSE */

	if ( padd & _PADk ) {   /* Added for PCMENU */
	    return 1;
	}

	if ( padd == opadd ) {
	}

	else if( opadd == 0 ) {
	    MENUidol( padd );
	}

#ifdef EMOUSE
	opadd = padd;
#endif /* EMOUSE */

	return 0;
}		

