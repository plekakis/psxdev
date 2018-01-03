/* $PSLibId: Run-time Library Release 4.4$ */
/*****************************************************************
 *
 * file: drawmax.c
 *
 * 	Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/
#include "libps.h"

#include "maxtypes.h"
#include "maxproto.h"

#include "psxload.h"
#include "spritex.h"

#include "maxstat.h"
extern MAXSTAT	maxstat;

extern int fot;

void do_max_cube( DB *db, DB **cdb, int nonstop )
{
    int		i;
    
    if( *cdb==db ) {                       	/* swap double buffer ID */
	*cdb = db+1;
	fot = 0;
    } else {
	*cdb = db;
	fot = 0;
    }
    ClearOTagR( (*cdb)->ot, OTSIZE );	/* clear ordering table */

    AddPrim( (*cdb)->ot+OTSIZE-1, &(*cdb)->bg );
    add_oval_prim( &(*cdb)->oval, (*cdb)->ot+OTSIZE-3 );

    if ( maxstat.pageNumber == -1 ) {
	for ( i = 0; i < maxstat.bgsprtCount; i++ ) {
	    if( maxstat.bgsprt[fot][i].priority == OTSIZE-2 ) continue;
	    AddSprite2( (*cdb), &maxstat.bgsprt[fot][i] );
	}

#ifndef EMOUSE
	MoveCursor( (*cdb), &maxstat.cursor[fot] );
#endif /* EMOUSE */
    }
}

void DrawDB( DB *cdb )
{
    VSNstatus(1);
    VSNdrawOTag( cdb->ot+OTSIZE-1, cdb->draw, cdb->disp );
}
