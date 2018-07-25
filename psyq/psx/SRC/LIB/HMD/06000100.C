/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>
#include "envmap.h"

/*
 * Shared Calculate Vertex and Normal (GsUSCAL2)
 * for HMD-ENV
 */
u_long *GsU_06000100(GsARGUNIT *ap)
{
	GsARGUNIT_SHARED *bp = (GsARGUNIT_SHARED *)ap;
	SVECTOR		*sv;
	DVECTOR		*dv;
	SVECTOR		*cv;
	GsWORKUNIT	*wu;
	u_long		work;
	int		num, flg, i, soffset, doffset;

	/*
	 * calculate vertex
	 */
	bp->primp++;
	num = *bp->primp;
	bp->primp++;
	soffset = *bp->primp;
	bp->primp++;
	doffset = *bp->primp;
	sv = bp->vertop + soffset;
	wu = bp->vertop2 + doffset;
	for (i = 0; i < num; i++,sv++,wu++) {
		gte_ldv0(sv);
		gte_rtps();
		gte_stflg(&flg);
		if (flg & 0x80000000) {
			*(u_long *)&wu->otz = 0xffffffff;
			continue;
		}
		gte_stsxy(&wu->vec);
		gte_stsz(&wu->otz);
		gte_stdp(&work);
		wu->p = (work&0xffff);
	}

	/*
	 * calculate normal
	 */
	bp->primp++;
	num = *bp->primp;
	bp->primp++;
	soffset = *bp->primp;
	bp->primp++;
	doffset = *bp->primp;
	sv = bp->nortop + soffset;
	cv = bp->nortop2 + doffset;
	for (i = 0; i < num; i++,sv++,cv++) {
#if 0	/* 1997/10/06 for Envmap */
		gte_ldv0(sv);
		gte_ncs();
		gte_stsv(cv);
#else
		*cv = *sv;
#endif
	}

	bp->primp++;
	return(bp->primp);
}

