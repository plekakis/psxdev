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
#include "localadd.h"
#include "mime.h"

/*
	JntRPYMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 25, 1997	N.Yoshioka.
*/

u_long *
GsU_04010011(GsARGUNIT *sp)
{
	GsARGUNIT_JntMIMe	*ap = (GsARGUNIT_JntMIMe *)sp;
	u_long		*pCoordSect	= ap->coord_sect;
	GsCOORDUNIT	*pCoord0	= (GsCOORDUNIT *)(pCoordSect + 1);
	long		*pMIMePr	= ap->mimepr;
	u_long		nMIMeNum	= ap->mimenum;
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
		u_long	nDflags = pMIMeDiff->nDflags;
		JntMIMeDiffData	*jntMIMeDiffData = pMIMeDiff->jntMIMeDiffData;
		u_short	j, key;
		u_short	changed = 0;
		LVECTOR	rvtmp;
		LVECTOR	rttmp;

		rvtmp.vx = rvtmp.vy = rvtmp.vz = 0;
		rttmp.vx = rttmp.vy = rttmp.vz = 0;

		j = 0;
		for (key = 0; j < nDiffsNum && key < nMIMeNum; key++) {
			/*
				For each JntMIMeDiffData...
			*/
			long	pr;

			if ((nDflags & (1 << key)) == 0) {
				continue;
			}

			pr = pMIMePr[key];

			if (pr != 0) {
				JntMIMeDiffData	*pMIMeDiffData
					= &jntMIMeDiffData[j];

				if (pMIMeDiffData->dtp & 0x01) {
					changed |= 0x01;
					rvtmp.vx += pMIMeDiffData->dvx * pr;
					rvtmp.vy += pMIMeDiffData->dvy * pr;
					rvtmp.vz += pMIMeDiffData->dvz * pr;
				}

				if (pMIMeDiffData->dtp & 0x02) {
					changed |= 0x02;
					rttmp.vx += pMIMeDiffData->dtx * pr;
					rttmp.vy += pMIMeDiffData->dty * pr;
					rttmp.vz += pMIMeDiffData->dtz * pr;
				}
			}

			j++;
		}

		if (changed) {
			/*
				Turn the flag in RstRPYMIMeData to non-zero
				if something was changed.
				And save the orginal coordinate data.
			*/
			RstRPYMIMeDiffData	*pRst
				= (RstRPYMIMeDiffData *)
					&jntMIMeDiffData[nDiffsNum];
			GsCOORDUNIT	*pCoord = &pCoord0[nCoordId];

			pRst->changed = changed;
			pRst->dvx = pCoord->rot.vx;
			pRst->dvy = pCoord->rot.vy;
			pRst->dvz = pCoord->rot.vz;
			pRst->dtx = pCoord->matrix.t[0];
			pRst->dty = pCoord->matrix.t[1];
			pRst->dtz = pCoord->matrix.t[2];

			if (changed & 0x01) {
				pCoord->rot.vx += rvtmp.vx / 4096;
				pCoord->rot.vy += rvtmp.vy / 4096;
				pCoord->rot.vz += rvtmp.vz / 4096;
				RotMatrix(&pCoord->rot, &pCoord->matrix);
			}

			if (changed & 0x02){
				pCoord->matrix.t[0] += rttmp.vx / 4096;
				pCoord->matrix.t[1] += rttmp.vy / 4096;
				pCoord->matrix.t[2] += rttmp.vz / 4096;
			}

			pCoord->flg = 0;
		}
	}

	/*
		Return the next primitive.
	*/
	return &((u_long *)pPrim)[pPrim->nSize];
}
