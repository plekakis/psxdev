/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>
#include "mime.h"

#include "localadd.h"

/****************************************
* 	gteMIMefunc	by Ohba		*
* 	a0=otp,a1=dfp,a2=n,a3=a(mime)	*
* 	VERTEX *otp, *dfp; int n,a;	*
************************************* ***/
#if 0

static void
mygteMIMefunc(SVECTOR *vs, SVECTOR *dv, long num, long p)
{
    long t0, t1, t2;
    SVECTOR *orgp=vs;
    SVECTOR *endp=vs+num;

    gte_ldlvl_0(p);

    for (; orgp!=endp; orgp++, dv++){
	t0= ((long *)dv)[0];
	t2= dv->vz;
	t1=t0>>16;
	t0=(t0<<16)>>16;
	gte_ldlvl_1(t0);
	gte_ldlvl_3(t2);
	t0=((long *)orgp)[0];
	t2=orgp->vz;
	gte_ldlvl_2(t1);
	gte_gpf12();
	t1=t0>>16;
	t0=(t0<<16)>>16;
	t0+=gte_stlvl_1();
	t1+=gte_stlvl_2();
	((long *)orgp)[0]= (t0&0xffff) | (t1<<16);
	t2+=gte_stlvl_3();
	orgp->vz=(short)t2;
    }
}

#else

extern void	GsVNMIMeFunc(SVECTOR *vec, SVECTOR *dvec, long np, long weight);
#define mygteMIMefunc	GsVNMIMeFunc

#endif

/*
	Vertices and Normal vectors common MIMe primitive driver.

		Aug 24, 1997	N.Yoshioka
*/

static u_long *
GsU_0401002x(GsARGUNIT_VNMIMe *ap, SVECTOR *pVNSect)
{
	long		*pMIMePr	= ap->mimepr;
	u_long		nMIMeNum	= ap->mimenum;
	/*u_short	nMIMeID		= ap->mimeid;*/
	u_long		*pMIMeDiffSect	= ap->mime_diff_sect;
	SVECTOR		*pOrgsVNSect	= ap->orgs_vn_sect;
	VNMIMePrim	*pPrim		= (VNMIMePrim *)ap->primp;
	int		num = pPrim->nNum;
	u_long		*idxMIMeDiff = pPrim->idxMIMeDiff;
	int		i;

	for (i = 0; i < num; i++) {
		/*
			For each VNMIMeDiff ptrs...
		*/
		VNMIMeDiff	*pMIMeDiff
			= (VNMIMeDiff *)&pMIMeDiffSect[idxMIMeDiff[i]];
		u_short	nOnum = pMIMeDiff->nOnum;
		u_short	nDiffsNum = pMIMeDiff->nDiffsNum;
		u_long	nDflags = pMIMeDiff->nDflags;
		u_long	*idxMIMeDiffData = pMIMeDiff->idxMIMeDiffData;
		u_short	j, key;
		u_short	changed = 0;

		j = 0;
		for (key = 0; j < nDiffsNum && key < nMIMeNum; key++) {
			/*
				For each VNMIMeDiffData ptrs...
			*/
			long	pr;

			if ((nDflags & (1 << key)) == 0) {
				continue;
			}

			pr = pMIMePr[key];

			if (pr != 0) {
				VNMIMeDiffData	*pMIMeDiffData
					= (VNMIMeDiffData *)
						&((u_long *)pMIMeDiff)
							[idxMIMeDiffData[j]];
				SVECTOR	*pVstart
					= &pVNSect[pMIMeDiffData->idxVstart];

				mygteMIMefunc(pVstart,
					pMIMeDiffData->svDiff,
					pMIMeDiffData->nVnum,
					pr);

				changed = 1;
			}

			j++;
		}

		if (changed) {
			/*
				Turn the flags in RstVNMIMe primitive to 1
				if something was changed.
			*/
			u_long	*idxVNMIMeChanged
				= (u_long *)&((u_long *)pMIMeDiff)
				[2 /* skip diffs_num, onum and dflags */
				+ nDiffsNum /* skip VNMIMeDiffData ptrs */];
			int	j;

			for (j = 0; j < nOnum; j++) {
				u_short	*p = (u_short *)&pMIMeDiffSect
						[idxVNMIMeChanged[j]];
				*p = 1;
			}
		}
	}

	/*
		Return the next primitive.
	*/
	return &((u_long *)pPrim)[pPrim->nSize];
}

/*
	VtxMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 24, 1997	N.Yoshioka
*/

u_long *
GsU_04010020(GsARGUNIT *sp)
{
	return GsU_0401002x((GsARGUNIT_VNMIMe *)sp,
		((GsARGUNIT_VNMIMe *)sp)->vert_sect);
}

/*
	NrmMIMe primitive driver.

		Originally written by S.Aoki.
		Aug 24, 1997	N.Yoshioka
*/

u_long *
GsU_04010021(GsARGUNIT *sp)
{
	return GsU_0401002x((GsARGUNIT_VNMIMe *)sp,
		((GsARGUNIT_VNMIMe *)sp)->norm_sect);
}
