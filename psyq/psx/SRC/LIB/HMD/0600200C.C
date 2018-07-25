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
 * Gouraud Triangle -> Gouraud Texture Triangle
 * for HMD-ENV (2D-ENV (reflection) for G3)
 */
u_long *GsU_0600200c(GsARGUNIT *sp)
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
	volatile POLY_G3  *si_org;
	volatile POLY_GT3 *si_env;
	VECTOR lv;
	int uu, vv;
	int ddx, ddy;
	GsARGUNIT_ENVMAP	*ep = (GsARGUNIT_ENVMAP *)sp;
	u_short			tpage_env; /* to scratch */
	u_short			clut_env; /* to scratch */
	u_char			reflect2;

	num = *(ap->primp)>>16;
	ap->primp++;
	tp = (HMD_P_G3 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	tpage_env = GetTPage(ep->p10, ep->p11, ((u_short *)(ep->tex2))[4],
	                    ((u_short *)(ep->tex2))[5]);

	if(ep->p10 != 2) {
		clut_env = GetClut(((u_short *)(ep->clut2))[4],((u_short *)(ep->clut2))[5]);
	}

	reflect2 = ep->p12;

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsENV *)scratch;
	scratch += sizeof(GsENV)/4;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;

	si_env = (POLY_GT3 *) ifo->pk;

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

		gte_avsz3();
		gte_stotz(&ifo->otz);

/* original g3 */

/* refraction gt3 */
		si_env->code = 0x34;	/* GT3, ABE=off, TGE=off */
		gte_stsxy3_gt3((u_long *) si_env);
		si_env->tpage = tpage_env;
		if(ep->p10 != 2) si_env->clut = clut_env;

		/* 1st vertex */
		gte_ApplyRotMatrix(&np[tp->n0], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT1;
		ddy = (int)lv.vy>>SHIFT1;
		uu = 128-((int)si_env->x0>>SHIFT2)+ddx;
		vv = 128+((int)si_env->y0>>SHIFT2)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u0 = uu;
		si_env->v0 = vv;
		si_env->r0 = reflect2;
		si_env->g0 = reflect2;
		si_env->b0 = reflect2;

		/* 2nd vertex */
		gte_ApplyRotMatrix(&np[tp->n1], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT1;
		ddy = (int)lv.vy>>SHIFT1;
		uu = 128-((int)si_env->x1>>SHIFT2)+ddx;
		vv = 128+((int)si_env->y1>>SHIFT2)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u1 = uu;
		si_env->v1 = vv;
		si_env->r1 = reflect2;
		si_env->g1 = reflect2;
		si_env->b1 = reflect2;

		/* 3rd vertex */
		gte_ApplyRotMatrix(&np[tp->n2], &lv);   /* normal->screen coord */
		ddx = (int)lv.vx>>SHIFT1;
		ddy = (int)lv.vy>>SHIFT1;
		uu = 128-((int)si_env->x2>>SHIFT2)+ddx;
		vv = 128+((int)si_env->y2>>SHIFT2)+ddy;
		if(uu < 0) uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0) vv = 0;
		if(vv > 255) vv = 255;
		si_env->u2 = uu;
		si_env->v2 = vv;
		si_env->r2 = reflect2;
		si_env->g2 = reflect2;
		si_env->b2 = reflect2;

		ifo->tag = (u_long *) (ifo->org + (ifo->otz >> ifo->shift));
		*(u_long *) si_env = (u_long)((*ifo->tag & 0x00ffffff) | 0x09000000);
		*ifo->tag = (u_long) si_env & 0x00ffffff;

		si_env++;
	}

	GsOUT_PACKET_P = (PACKET *)si_env;
	return(ap->primp+1);
}
