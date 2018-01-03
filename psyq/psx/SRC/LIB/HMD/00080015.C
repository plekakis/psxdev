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
	u_short	uv0, cba;
	u_short	uv1, tsb;
	u_short	uv2, n0;
	u_short	uv3, v0;
	u_short n1, v1;
	u_short n2, v2;
	u_short n3, v3;
}	HMD_P_GT4;

/*
 * Active Sub-divide Gouraud Texture Quad (GsU_00080015)
 */
u_long *GsU_00080015(GsARGUNIT *sp)
{
	register GsADIV_GT4	*ifo;
	register HMD_P_GT4	*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
	GsADIV_P_GT4		*p1;
	POLY_GT4		*si;
	u_long			*scratch = (u_long *)(ap+1);
	int			i;
	int			num;
	static void 		Adiv_gt4();

	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_GT4 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsADIV_GT4 *)scratch;
	scratch += sizeof(GsADIV_GT4)/4;
	p1 = (GsADIV_P_GT4 *)scratch;

	ifo->limit = (u_long)(*(ap->primp)>>24);
	ifo->hwd0 = HWD0/2;
	ifo->vwd0 = VWD0/2;
	ifo->adivz = GsADIVZ;
	ifo->adivw = GsADIVW;
	ifo->adivh = GsADIVH;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;
	setPolyGT4(&ifo->si);
	setRGB0(&ifo->si, 0x80, 0x80, 0x80);
	gte_ldrgb(&ifo->si.r0);

	si = (POLY_GT4 *)ifo->pk;

	for (i = 0; i < num; i++, tp++) {
		gte_ldv3(&vp[tp->v0], &vp[tp->v1], &vp[tp->v2]);

		gte_rtpt();

		*(u_long *)&si->u0 = *(u_long *)&tp->uv0;
		*(u_long *)&si->u2 = *(u_long *)&tp->uv2;

		gte_stflg(&ifo->flg0);
			
		gte_nclip();
		*(u_long *)&si->u3 = *(u_long *)&tp->uv3;
		gte_stopz(&ifo->otz);

		if (ifo->otz <= 0) {
			continue;
		}

		gte_stsxy3_gt4((u_long *)si);
		gte_ldv0(&vp[tp->v3]);

		gte_rtps();
		gte_stflg(&ifo->flg);

		if ((ifo->flg & 0x00040000) && (ifo->flg0 & 0x00040000)) {
			continue;
		}

		gte_stsxy((u_long *)&si->x3);
		gte_avsz4();
		gte_stotz(&ifo->otz);
		gte_ldv3(&np[tp->n0], &np[tp->n1], &np[tp->n2]);
		gte_ncct();

		*(u_long *)&si->u1 = *(u_long *)&tp->uv1;

		ifo->tag = (u_long *)(ifo->org + (ifo->otz >> ifo->shift));
		gte_strgb3(&si->r0, &si->r1, &si->r2);

		gte_ldv0(&np[tp->n3]);
		gte_nccs();
		gte_strgb(&si->r3);

		if (ifo->otz < ifo->adivz) {
			goto divide;
		}
			
		if ((ifo->flg & 0x80000000) || (ifo->flg0 & 0x80000000)) {
			goto divide;
		}
			
		*(u_long *)si = (*ifo->tag & 0x00ffffff) | 0x0c000000;
		*ifo->tag = (u_long)si&0x00ffffff;

		si++;
		continue;

divide:
		*(SVECTOR *)&p1->vt[0] = vp[tp->v0];
		*(SVECTOR *)&p1->vt[1] = vp[tp->v1];
		*(SVECTOR *)&p1->vt[2] = vp[tp->v2];
		*(SVECTOR *)&p1->vt[3] = vp[tp->v3];
		*(u_long *)&p1->vt[0].col.r = *(u_long *)&si->r0;
		*(u_long *)&p1->vt[1].col.r = *(u_long *)&si->r1;
		*(u_long *)&p1->vt[2].col.r = *(u_long *)&si->r2;
		*(u_long *)&p1->vt[3].col.r = *(u_long *)&si->r3;
		*(u_short *)&p1->vt[0].tu = tp->uv0;
		*(u_short *)&p1->vt[1].tu = tp->uv1;
		*(u_short *)&p1->vt[2].tu = tp->uv2;
		*(u_short *)&p1->vt[3].tu = tp->uv3;

		ifo->si.clut = tp->cba;
		ifo->si.tpage = tp->tsb;

		ifo->pk = (u_long *)si;

		Adiv_gt4(scratch, ifo, 0);

		si = (POLY_GT4 *)ifo->pk;
		continue;
	}
	GsOUT_PACKET_P = (PACKET *)si;
	return(ap->primp+1);
}

#include "datg4_cm.c"
