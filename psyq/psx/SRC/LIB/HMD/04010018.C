/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>
#include "mime.h"

/*
	RstJntAxesMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 25, 1997	N.Yoshioka.
*/

u_long *
GsU_04010018(GsARGUNIT *sp)
{
	GsARGUNIT_RstJntMIMe	*ap = (GsARGUNIT_RstJntMIMe *)sp;
	u_long		*pCoordSect	= ap->coord_sect;
	GsCOORDUNIT	*pCoord0	= (GsCOORDUNIT *)(pCoordSect + 1);
	/*u_short	nMIMeID		= ap->mimeid;*/
	u_long		*pMIMeDiffSect	= ap->mime_diff_sect;
	JntMIMePrim	*pPrim		= (JntMIMePrim *)ap->primp;
	int		num = pPrim->nNum;
	u_long		*idxMIMeDiff = pPrim->idxMIMeDiff;
	int		i;

	for (i = 0; i < num; i++) {
		/*
			For each JntMIMeDiff ptrs...
		*/
		JntMIMeDiff	*pMIMeDiff
			= (JntMIMeDiff *)&pMIMeDiffSect[idxMIMeDiff[i]];
		u_short	nCoordId = pMIMeDiff->nCoordId;
		u_short	nDiffsNum = pMIMeDiff->nDiffsNum;
		JntMIMeDiffData	*jntMIMeDiffData = pMIMeDiff->jntMIMeDiffData;
		RstAxesMIMeDiffData	*pRst
			= (RstAxesMIMeDiffData *)&jntMIMeDiffData[nDiffsNum];

		if (pRst->changed) {
			GsCOORDUNIT	*pCoord = &pCoord0[nCoordId];
			int	m, n;

			for (m = 0; m < 3; m++) {
				for (n = 0; n < 3; n++) {
					pCoord->matrix.m[m][n] = pRst->m[m][n];
				}
			}
			pCoord->matrix.t[0] = pRst->dtx;
			pCoord->matrix.t[1] = pRst->dty;
			pCoord->matrix.t[2] = pRst->dtz;
			pRst->changed = 0;

			pCoord->flg = 0;
		}
	}

	/*
		Return the next primitive.
	*/
	return &((u_long *)pPrim)[pPrim->nSize];
}
