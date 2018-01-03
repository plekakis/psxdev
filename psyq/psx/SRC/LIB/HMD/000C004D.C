/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <inline_c.h>
#include <gtemac.h>
#include <libgpu.h>
#include <libgs.h>
#include <asm.h>
#include <libhmd.h>

typedef struct {
	u_char	r0, g0, b0, code;
	u_char	r1, g1, b1, pad0;
	u_char	r2, g2, b2, pad1;
	u_short	uv0, cba;
	u_short	uv1, tsb;
	u_short	uv2, v0;
	u_short v1, v2;
}	HMD_P_NGT3;

/*
 * Active Sub-divide Non-light Gouraud Texture Triangle (GsU_000c004d)
 */
u_long *GsU_000c004d(GsARGUNIT *sp)
{
	register GsADIV_GT3	*ifo;
	register HMD_P_NGT3	*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
	GsADIV_P_GT3		*p1;
	POLY_GT3		*si;
	u_long			*scratch = (u_long *)(ap+1);
	int			i;
	int			num;
	static void 		Adiv_gt3();

	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_NGT3 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsADIV_GT3 *)scratch;
	scratch += sizeof(GsADIV_GT3)/4;
	p1 = (GsADIV_P_GT3 *)scratch;

	ifo->limit = (u_long)(*(ap->primp)>>24);
	ifo->hwd0 = HWD0/2;
	ifo->vwd0 = VWD0/2;
	ifo->adivz = GsADIVZ;
	ifo->adivw = GsADIVW;
	ifo->adivh = GsADIVH;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;
	setPolyGT3(&ifo->si);

	si = (POLY_GT3 *)ifo->pk;

	for (i = 0; i < num; i++, tp++) {
		gte_ldv3(&vp[tp->v0], &vp[tp->v1], &vp[tp->v2]);

		gte_rtpt();

		*(u_long *)&si->u0 = *(u_long *)&tp->uv0;
		*(u_long *)&si->u1 = *(u_long *)&tp->uv1;

		gte_stflg(&ifo->flg);
			
		gte_nclip();

		*(u_long *)&si->u2 = *(u_long *)&tp->uv2;

		gte_stopz(&ifo->otz);

		if (ifo->otz <= 0) {
			continue;
		}
		if (ifo->flg & 0x00040000) {
			continue;
		}

		gte_stsxy3_gt3((u_long *)si);
		gte_avsz3();

		*(u_long *)&si->r0 = *(u_long *)&tp->r0;

		gte_stotz(&ifo->otz);

		*(u_long *)&si->r1 = *(u_long *)&tp->r1;
		*(u_long *)&si->r2 = *(u_long *)&tp->r2;

		ifo->tag = (u_long *)(ifo->org + (ifo->otz >> ifo->shift));

		if(ifo->otz < ifo->adivz) {
			goto divide;
		}
			
		if(ifo->flg & 0x80000000) {
			goto divide;
		}
			
		*(u_long *)si = (*ifo->tag & 0x00ffffff) | 0x09000000;
		*ifo->tag = (u_long)si&0x00ffffff;

		si++;
		continue;

divide:
		*(SVECTOR *)&p1->vt[0] = vp[tp->v0];
		*(SVECTOR *)&p1->vt[1] = vp[tp->v1];
		*(SVECTOR *)&p1->vt[2] = vp[tp->v2];
		*(u_long *)&p1->vt[0].col.r = *(u_long *)&si->r0;
		*(u_long *)&p1->vt[1].col.r = *(u_long *)&si->r1;
		*(u_long *)&p1->vt[2].col.r = *(u_long *)&si->r2;
		*(u_short *)&p1->vt[0].tu = tp->uv0;
		*(u_short *)&p1->vt[1].tu = tp->uv1;
		*(u_short *)&p1->vt[2].tu = tp->uv2;

		ifo->si.clut = tp->cba;
		ifo->si.tpage = tp->tsb;

		ifo->pk = (u_long *)si;

		Adiv_gt3(scratch, ifo, 0);

		si = (POLY_GT3 *)ifo->pk;
		continue;
	}
	GsOUT_PACKET_P = (PACKET *)si;
	return(ap->primp+1);
}

#include "datg3_cm.c"
