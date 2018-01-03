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

#if 0

/*
	Version 1, the original code.
*/
static void
GsRestoreOrgsVNMIMe(RstVNMIMePrim *pPrim,
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

			if (pData->nChanged) {
				for (k = 0 ; k < pData->nVnum; k++) {
					vn[pData->idxVstart + k]
						= orgs[pData->idxOstart + k];
				}
				pData->nChanged = 0;
			}
		}
	}
}

#elif 0

/*
	Version 2, faster than version 1.
	But now, we use assembler version.
*/
static void
GsRestoreOrgsVNMIMe(RstVNMIMePrim *pPrim,
	u_long *pDiffSect, SVECTOR *orgs, SVECTOR *vn)
{
	/* the SCANing flag is already 0 at this point */
	int	n;
	RstVNMIMeDiff	*pDiff;
	int	j;
	int	idxDiff;
	RstVNMIMeDiffData	*pData;
	int	k;
	u_long	*vnp, *orgsp;
	u_long	*idxDiffp;

	n = pPrim->n;
	idxDiffp = &pPrim->idxDiff[n];
	while (--n >= 0) {
		idxDiffp--;
		idxDiff = *idxDiffp;
		pDiff = (RstVNMIMeDiff *)&pDiffSect[idxDiff];
		j = pDiff->nDiffsNum;
		pData = &pDiff->diffData[j];
		while (--j >= 0) {
			pData--;
			if (pData->nChanged) {
				pData->nChanged = 0;
				k = pData->nVnum * 2;
				vnp = &((u_long *)vn)[2 * pData->idxVstart + k];
				orgsp = &((u_long *)orgs)[2 * pData->idxOstart + k];
				while (--k >= 0) {
					vnp--;
					orgsp--;
					*vnp = *orgsp;
				}
			}
		}
	}
}

#endif

/*
	RstVtxMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 24,1997	N.Yoshioka.
*/

u_long *
GsU_04010028(GsARGUNIT *sp)
{
	GsARGUNIT_RstVNMIMe	*ap = (GsARGUNIT_RstVNMIMe *)sp;
	u_long		*pMIMeDiffSect	= ap->mime_diff_sect;
	SVECTOR		*pOrgsVNSect	= ap->orgs_vn_sect;
	SVECTOR		*pVNSect	= ap->vert_sect;
	RstVNMIMePrim	*pPrim		= (RstVNMIMePrim *)ap->primp;

	GsRestoreOrgsVNMIMe(pPrim,
		pMIMeDiffSect,	/* MIMeDiffSect ptr */
		pOrgsVNSect,	/* OrgsVNSect ptr */
		pVNSect		/* VNSect ptr */);

	return &((u_long *)pPrim)[pPrim->nSize];
}

/*
	RstNrmMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 24,1997	N.Yoshioka.
*/

u_long *
GsU_04010029(GsARGUNIT *sp)
{
	GsARGUNIT_RstVNMIMe	*ap = (GsARGUNIT_RstVNMIMe *)sp;
	u_long		*pMIMeDiffSect	= ap->mime_diff_sect;
	SVECTOR		*pOrgsVNSect	= ap->orgs_vn_sect;
	SVECTOR		*pVNSect	= ap->norm_sect;
	RstVNMIMePrim	*pPrim		= (RstVNMIMePrim *)ap->primp;

	GsRestoreOrgsVNMIMe(pPrim,
		pMIMeDiffSect,	/* MIMeDiffSect ptr */
		pOrgsVNSect,	/* OrgsVNSect ptr */
		pVNSect		/* VNSect ptr */);

	return &((u_long *)pPrim)[pPrim->nSize];
}
