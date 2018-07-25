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
 * Gouraud Triangle -> Flat Texture Triangle
 * for HMD-ENV (1D-ENV for G3)
 */
u_long *GsU_0600100c(GsARGUNIT *sp)
{
	register GsENV		*ifo;
	register HMD_P_G3	*tp;
	GsARGUNIT_ENVMAP	*ap = (GsARGUNIT_ENVMAP *)sp;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
	u_long			*scratch = (u_long *)(ap+1);
	int			i;
	int			num;
	/* added variable */
	volatile POLY_FT3 *si_env;
	VECTOR lv;
	GsARGUNIT_ENVMAP	*ep = (GsARGUNIT_ENVMAP *)sp;
	u_short			tpage; /* to scratch */
	u_char			material2;
	int y;

	num = *(ap->primp)>>16;
	ap->primp++;
	tp = (HMD_P_G3 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	tpage = GetTPage(ep->p00, 0, ((u_short *)(ep->tex1))[4],
	                 ((u_short *)(ep->tex1))[5]);
	material2 = ep->p02;
	y = material2 + (((u_short *)(ep->tex1))[5])%256;

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsENV *)scratch;
	scratch += sizeof(GsENV)/4;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;

	si_env = (POLY_FT3 *) ifo->pk;

	for (i = 0; i < num; i++, tp++) {
		gte_ldv3(&vp[tp->v0], &vp[tp->v1], &vp[tp->v2]);
		gte_rtpt();			/* RotTransPers3 */
		gte_stflg(&ifo->flg);
		if (ifo->flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		gte_stopz(&ifo->otz);	/* back clip */
		if (ifo->otz <= 0)
			continue;
		gte_stsxy3_ft3((u_long *) si_env);
		gte_avsz3();
		gte_stotz(&ifo->otz);

		setPolyFT3(si_env);
		setRGB0(si_env,0x80,0x80,0x80);	/* default brightness */
		si_env->tpage = tpage;

		/* UV calcurate */
		gte_ApplyRotMatrix(&np[tp->n0], &lv);
		si_env->u0 = ((int)lv.vy/32+128)&0xff;
		si_env->v0 = y;

		gte_ApplyRotMatrix(&np[tp->n1], &lv);
		si_env->u1 = ((int)lv.vy/32+128)&0xff;
		si_env->v1 = y;

		gte_ApplyRotMatrix(&np[tp->n2], &lv);
		si_env->u2 = ((int)lv.vy/32+128)&0xff;
		si_env->v2 = y;

		ifo->tag = (u_long *) (ifo->org + (ifo->otz >> ifo->shift));
		*(u_long *) si_env = (u_long)((*ifo->tag & 0x00ffffff) | 0x07000000);
		*ifo->tag = (u_long) si_env & 0x00ffffff;
		si_env++;
	}

	GsOUT_PACKET_P = (PACKET *)si_env;
	return(ap->primp+1);
}
