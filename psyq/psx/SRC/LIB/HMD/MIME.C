/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>
#include "mime.h"

static void
GsCopyOrgsVNMIMe(RstVNMIMePrim *pPrim,
	u_long *pDiffSect, SVECTOR *orgs, SVECTOR *vn)
{
	/* the SCANing flag is already 0 at this point */
	int	n = pPrim->n;
	int	i;

	for (i = 0; i < n; i++) {
		RstVNMIMeDiff	*pDiff =
			(RstVNMIMeDiff *)&pDiffSect[pPrim->idxDiff[i]];
		int	j;

		for (j = 0; j < pDiff->nDiffsNum; j++) {
			RstVNMIMeDiffData	*pData = &pDiff->diffData[j];
			int	k;

			for (k = 0 ; k < pData->nVnum; k++) {
				orgs[pData->idxOstart + k]
					= vn[pData->idxVstart + k];
			}
		}
	}
}

/*
	Initialization routine for vertices MIMe.
	At this point, this routine performs just to save
	original vertices data.
*/

void
GsInitRstVtxMIMe(u_long *pPrim, u_long *pHdr)
{
	GsCopyOrgsVNMIMe((RstVNMIMePrim *)(pPrim + 1 /* skip type */),
		(u_long *)pHdr[2],	/* MIMeDiffSect ptr */
		(SVECTOR *)pHdr[3],	/* OrgsVNSect ptr */
		(SVECTOR *)pHdr[4]	/* VertSect ptr */);
}

/*
	Initialization routine for normal vectors MIMe.
	At this point, this routine performs just to save
	original normal vectors data.
*/

void
GsInitRstNrmMIMe(u_long *pPrim, u_long *pHdr)
{
	GsCopyOrgsVNMIMe((RstVNMIMePrim *)(pPrim + 1 /* skip type */),
		(u_long *)pHdr[2],	/* MIMeDiffSect ptr */
		(SVECTOR *)pHdr[3],	/* OrgsVNSect ptr */
		(SVECTOR *)pHdr[5]	/* NormSect ptr */);
}
