/* $PSLibId: Run-time Library Release 4.4$ */
/*			chain: chained CdRead()
 *			
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Jul.25,1995	suzu
*/
/*			     Chained CdRead
*/
/*	CdReadChain() calls CdRead() automatically when the previous CdRead()
 *	is finished. The reading parameters should be predetermined and
 *	defined in the arrays which is sent cdReadChain() as an argument.
 *	
 *--------------------------------------------------------------------------
 *	
 * CdReadChain		Read many files from CD-ROM in background.
 *
 * SYNOPSIS
 *	int cdReadChain(CdlLOC *postbl, int *scttbl, u_long **buftbl, int ntbl)
 *
 * ARGUMENT
 *	postbl	array stored the position of files to be read.
 *	scttbl	array stored the number of sectors to be read.
 *	buftbl	array stored the address of main memory to be loaded.
 *	ntbl	number of elements of each array
 *
 * DESCRIPTION
 *	cdReadChain reads may files from CD-ROM and loads the contents
 *	on the different address of the main memory.
 *	Each reading operations is kicked automatically in the
 * 	CdReadCallback by The end of the previous reading operation.
 *	Reading position, sector size and buffer adresses are set in
 *	each array. The end of the whole reading can be detected by
 *	CdReadChainSync() .
 *
 * NOTE	
 * 	cdReadChain() uses CdReadCallback().
 *	If any read error is detected, cdReaChain retry from the top
 *	of the file list.
 *	
 * RETURN
 *	always 0
 *--------------------------------------------------------------------------
 *	
 * cdReadChainSync	get status of CdReadChain()
 *
 * SYNOPSIS
 *	int cdReadChainSync(void)
 *
 * ARGUMENT
 *	none
 *
 * DESCRITION
 *	cdReadChainSync returns the number of file to be left.
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
#include <libcd.h>

static CdlLOC	*postbl;	/* position table */
static int	*scttbl;	/* sector number table */
static u_long	**buftbl;	/* destination buffer pointer table */
static int	ctbl;		/* current CdRead */
static int	ntbl;		/* total CdReads */

void cbread(u_char intr, u_char *result);
		   
int cdReadChainSync()
{
	return(ntbl - ctbl);
}
	    
int cdReadChain(CdlLOC *_postbl, int *_scttbl, u_long **_buftbl, int _ntbl)
{
	unsigned char com;

	/* save pointers */
	postbl = _postbl;
	scttbl = _scttbl;
	buftbl = _buftbl;
	ntbl   = _ntbl;
	ctbl   = 0;
	
	CdReadCallback(cbread);
	CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
	com = CdlModeSpeed;
	CdControlB( CdlSetmode, &com, 0 );
	VSync( 3 );
	CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
	return(0);
}

void cbread(u_char intr, u_char *result)
{
	/*printf("cbread*/
	if (intr == CdlComplete) {
		if (++ctbl == ntbl)
			CdReadCallback(0);
		else {
			CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
			CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
		}
	}
	else {
		printf("cdReadChain: data error\n");
		ctbl = 0;
		CdControl(CdlSetloc, (u_char *)&postbl[ctbl], 0);
		CdRead(scttbl[ctbl], buftbl[ctbl], CdlModeSpeed);
	}
}
		
		

