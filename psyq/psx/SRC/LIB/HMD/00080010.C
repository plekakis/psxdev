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
	u_short n0,  v0;
	u_short v1,  v2;
	u_short v3,  pad;
}	HMD_P_F4;

/*
 * Active Sub-divide Flat Quad (GsU_00080010)
 */
u_long *GsU_00080010(GsARGUNIT *sp)
{
	register GsADIV_F4	*ifo;
	register HMD_P_F4	*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
	GsADIV_P_F4		*p1;
	POLY_F4			*si;
	u_long			*scratch = (u_long *)(ap+1);
	int			i;
	int			num;
	static void 		Adiv_f4();

	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_F4 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsADIV_F4 *)scratch;
	scratch += sizeof(GsADIV_F4)/4;
	p1 = (GsADIV_P_F4 *)scratch;

	ifo->limit = (u_long)(*(ap->primp)>>24);
	ifo->hwd0 = HWD0/2;
	ifo->vwd0 = VWD0/2;
	ifo->adivz = GsADIVZ;
	ifo->adivw = GsADIVW;
	ifo->adivh = GsADIVH;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;
	setPolyF4(&ifo->si);

	si = (POLY_F4 *)ifo->pk;

	for (i = 0; i < num; i++, tp++) {
		gte_ldrgb(&tp->r0);
		gte_ldv3(&vp[tp->v0], &vp[tp->v1], &vp[tp->v2]);

		gte_rtpt();
		gte_stflg(&ifo->flg0);
			
		gte_nclip();
		gte_stopz(&ifo->otz);

		if (ifo->otz <= 0) {
			continue;
		}

		gte_ldv0(&np[tp->n0]);	/* lighting */
		gte_nccs();

		gte_stsxy3_f4((u_long *)si);

		gte_ldv0(&vp[tp->v3]);
		gte_rtps();

		gte_stflg(&ifo->flg);

		if ((ifo->flg & 0x00040000) && (ifo->flg0 & 0x00040000)) {
			continue;
		}

		gte_stsxy((u_long *)&si->x3);

		gte_avsz4();
		gte_stotz(&ifo->otz);
		ifo->tag = (u_long *)(ifo->org + (ifo->otz >> ifo->shift));

		if (ifo->otz < ifo->adivz) {
			goto divide;
		}
			
		if ((ifo->flg & 0x80000000) && (ifo->flg0 & 0x80000000)) {
			goto divide;
		}
			
		*(u_long *)si = (*ifo->tag & 0x00ffffff) | 0x05000000;
		*ifo->tag = (u_long)si&0x00ffffff;

		gte_strgb(&si->r0);
		si++;
		continue;

divide:
		gte_strgb(&ifo->si.r0);
		*(SVECTOR *)&p1->vt[0] = vp[tp->v0];
		*(SVECTOR *)&p1->vt[1] = vp[tp->v1];
		*(SVECTOR *)&p1->vt[2] = vp[tp->v2];
		*(SVECTOR *)&p1->vt[3] = vp[tp->v3];

		ifo->pk = (u_long *)si;

		Adiv_f4(scratch, ifo, 0);

		si = (POLY_F4 *)ifo->pk;
		continue;
	}
	GsOUT_PACKET_P = (PACKET *)si;
	return(ap->primp+1);
}

#include "daf4_cm.c"
