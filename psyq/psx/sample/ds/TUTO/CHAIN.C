/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			chain: chained DsRead()
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.25,1995	suzu
 *		2.00		Apr.25,1997	makoto
*/
/*			     Chained DsRead
 *
 *	dsReadChain() calls DsRead() automatically when the previous DsRead()
 *	is finished. The reading parameters should be predetermined and
 *	defined in the arrays which is sent dsReadChain() as an argument.
 *
 *--------------------------------------------------------------------------
 *
 * dsReadChain		Read many files from CD-ROM in background.
 *
 * SYNOPSIS
 *	int dsReadChain( DslLOC* postbl, int* scttbl, u_long** buftbl,
 *	  int ntbl )
 *
 * ARGUMENT
 *	postbl	array stored the position of files to be read.
 *	scttbl	array stored the number of sectors to be read.
 *	buftbl	array stored the address of main memory to be loaded.
 *	ntbl	number of elements of each array
 *
 * DESCRIPTION
 *	dsReadChain reads may files from CD-ROM and loads the contents
 *	on the different address of the main memory.
 *	Each reading operations is kicked automatically in the
 * 	DsReadCallback by The end of the previous reading operation.
 *	Reading position, sector size and buffer adresses are set in
 *	each array. The end of the whole reading can be detected by
 *	dsReadChainSync() .
 *
 * NOTE
 * 	dsReadChain() uses DsReadCallback().
 *	If any read error is detected, cdReaChain retry from the top
 *	of the file list.
 *
 * RETURN
 *	always 0
 *--------------------------------------------------------------------------
 *
 * dsReadChainSync	get status of dsReadChain()
 *
 * SYNOPSIS
 *	int dsReadChainSync( void )
 *
 * ARGUMENT
 *	none
 *
 * DESCRITION
 *	dsReadChainSync returns the number of file to be left.
 *
 * RETURN
 *	plus 	number of file to be read.
 *	0	normally terminated
 *	-1	error detected
 *--------------------------------------------------------------------------
 * */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

static DslLOC *postbl;		/* position table */
static int* scttbl;		/* sector number table */
static u_long** buftbl;		/* destination buffer pointer table */
static int ctbl;		/* current DsRead */
static int ntbl;		/* total DsReads */

static void cbread( u_char intr, u_char* result );

int dsReadChainSync( void )
{
	return ntbl - ctbl;
}

int dsReadChain( DslLOC* _postbl, int* _scttbl, u_long** _buftbl, int _ntbl )
{
	unsigned char com;

	/* save pointers */
	postbl = _postbl;
	scttbl = _scttbl;
	buftbl = _buftbl;
	ntbl = _ntbl;
	ctbl = 0;

	DsReadCallback( cbread );
	DsRead( &postbl[ ctbl ], scttbl[ ctbl ], buftbl[ ctbl ],
	  DslModeSpeed );
	return 0;
}

static void cbread( u_char intr, u_char* result )
{
/*	printf( "cbread: (%s)...\n", DsIntstr( intr ) ); */
	if( intr == DslComplete ) {
		if( ++ctbl == ntbl )
			DsReadCallback( 0 );
		else {
			DsRead( &postbl[ ctbl ], scttbl[ ctbl ],
			  buftbl[ ctbl ], DslModeSpeed );
		}
	} else {
		printf( "dsReadChain: data error\n" );
		ctbl = 0;
		DsRead( &postbl[ ctbl ], scttbl[ ctbl ], buftbl[ ctbl ],
		  DslModeSpeed );
	}
}
