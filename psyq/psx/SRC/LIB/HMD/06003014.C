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
 * Gouraud Quad -> Gouraud Texture Quad
 * for HMD-ENV (2D-ENV (reflection) for G4)
 */
u_long *GsU_06003014(GsARGUNIT *sp)
{
	register GsENV		*ifo;
	register HMD_P_G4	*tp;
	GsARGUNIT_ENVMAP	*ap = (GsARGUNIT_ENVMAP *)sp;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
	u_long			*scratch = (u_long *)(ap+1);
	int			i;
	int			num;
	/* added variable */
	volatile POLY_GT4 *si_org;
	VECTOR lv;
	int uu, vv;
	int ddx, ddy;

	num = *(ap->primp)>>16;
	ap->primp++;
	tp = (HMD_P_G4 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	/* parameters write to scratch pad (initilaize) */
	ifo = (GsENV *)scratch;
	scratch += sizeof(GsENV)/4;
	ifo->pk = (u_long *)ap->out_packetp;
	ifo->org = (u_long *)ap->tagp->org;
	ifo->shift = ap->shift;

	ifo->tpage_org = GetTPage(ap->p20, ap->p21, ((u_short *)(ap->tex3))[4],
	                    ((u_short *)(ap->tex3))[5]);
	if(ap->p20 != 2) {
		ifo->clut_org = GetClut(((u_short *)(ap->clut3))[4],((u_short *)(ap->clut3))[5]);
	}
	ifo->refract = ap->p22;

	si_org = (POLY_GT4 *) ifo->pk;

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
		gte_stsxy3_gt4((u_long *) si_org); /**/
		gte_ldv0(&vp[tp->v3]);
		gte_rtps();
		gte_stflg(&ifo->flg);
		if ((ifo->flg & 0x00040000) && (ifo->flg0 & 0x00040000)) {
			continue;
		}
		gte_stsxy((u_long *)&si_org->x3);
		gte_avsz4();
		gte_stotz(&ifo->otz);

/* Original polygon */
		si_org->code = 0x3C;	/* GT4, ABE=off, TGE=off */
		si_org->tpage = ifo->tpage_org;
		if(ap->p20 != 2) si_org->clut = ifo->clut_org;

		/* 1st vertex */
		gte_ApplyRotMatrix(&np[tp->n0], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*ifo->refract>>24):-((lv.vx*lv.vx)*ifo->refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*ifo->refract>>24):-((lv.vy*lv.vy)*ifo->refract>>24);
		uu = 128+((int)si_org->x0>>1)+ddx;
		vv = 128+((int)si_org->y0)-ddy;
		if(uu < 0)   uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0)   vv = 0;
		if(vv > 255) vv = 255;
		si_org->u0 = uu;
		si_org->v0 = vv;
		si_org->r0 = ap->p25;
		si_org->g0 = ap->p26;
		si_org->b0 = ap->p27;

		/* 2st vertex */
		gte_ApplyRotMatrix(&np[tp->n1], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*ifo->refract>>24):-((lv.vx*lv.vx)*ifo->refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*ifo->refract>>24):-((lv.vy*lv.vy)*ifo->refract>>24);
		uu = 128+((int)si_org->x1>>1)+ddx;
		vv = 128+((int)si_org->y1)-ddy;
		if(uu < 0)   uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0)   vv = 0;
		if(vv > 255) vv = 255;
		si_org->u1 = uu;
		si_org->v1 = vv;
		si_org->r1 = ap->p25;
		si_org->g1 = ap->p26;
		si_org->b1 = ap->p27;

		/* 3st vertex */
		gte_ApplyRotMatrix(&np[tp->n2], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*ifo->refract>>24):-((lv.vx*lv.vx)*ifo->refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*ifo->refract>>24):-((lv.vy*lv.vy)*ifo->refract>>24);
		uu = 128+((int)si_org->x2>>1)+ddx;
		vv = 128+((int)si_org->y2)-ddy;
		if(uu < 0)   uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0)   vv = 0;
		if(vv > 255) vv = 255;
		si_org->u2 = uu;
		si_org->v2 = vv;
		si_org->r2 = ap->p25;
		si_org->g2 = ap->p26;
		si_org->b2 = ap->p27;

		/* 4st vertex */
		gte_ApplyRotMatrix(&np[tp->n3], &lv);   /* normal->screen coord */
		ddx = lv.vx>0?((lv.vx*lv.vx)*ifo->refract>>24):-((lv.vx*lv.vx)*ifo->refract>>24);
		ddy = lv.vy>0?((lv.vy*lv.vy)*ifo->refract>>24):-((lv.vy*lv.vy)*ifo->refract>>24);
		uu = 128+((int)si_org->x3>>1)+ddx;
		vv = 128+((int)si_org->y3)-ddy;
		if(uu < 0)   uu = 0;
		if(uu > 255) uu = 255;
		if(vv < 0)   vv = 0;
		if(vv > 255) vv = 255;
		si_org->u3 = uu;
		si_org->v3 = vv;
		si_org->r3 = ap->p25;
		si_org->g3 = ap->p26;
		si_org->b3 = ap->p27;

/* Reflection polygon */

		ifo->tag = (u_long *) (ifo->org + (ifo->otz >> ifo->shift));
		*(u_long *) si_org = (u_long)((*ifo->tag & 0x00ffffff) | 0x0C000000);
		*ifo->tag = (u_long) si_org & 0x00ffffff;

		si_org++;
	}

	GsOUT_PACKET_P = (PACKET *)si_org;
	return(ap->primp+1);
}
