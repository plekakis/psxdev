/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#define NO_PROTOTYPE

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>

typedef struct {
	u_char	r0, g0, b0, code0;
	u_char	r1, g1, b1, code1;
	u_char	r2, g2, b2, code2;
	u_short uv0, clut;
	u_short uv1, tpage;
	u_short uv2, v0;
	u_short v1, v2;
}       HMD_P_NGT3;

/*
 * Divide Non-light Gouraud Texture Triangle (GsUDNGT3)
 */
u_long *GsU_0005004d(GsARGUNIT *sp)
{
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	POLY_GT3		*pp;
	HMD_P_NGT3		*tp;
	DIVPOLYGON3		*divp = (DIVPOLYGON3 *)(&(ap->nortop)+1);
  	RVECTOR			*r0, *r1, *r2;
	SVECTOR			*vp = ap->vertop;
  	int			i;
  	u_short			sz;
  	long			p, flag;
  	CRVECTOR3		*crv;
  	long			lv;
	int			num;

	pp = (POLY_GT3 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_NGT3 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	divp->ndiv = *(ap->primp)>>24;
	divp->pih = HWD0;
	divp->piv = VWD0;

  	r0= &divp->r0;
  	r1= &divp->r1;
  	r2= &divp->r2;
  	lv=0;
  	crv = divp->cr;
  	divp->cr[lv].r0 = r0;		
  	divp->cr[lv].r1 = r1;		
  	divp->cr[lv].r2 = r2;		
  
  	for (i = 0; i < num; i++, tp++) {
		r0->v = *(vp + tp->v0);
		r1->v = *(vp + tp->v1);
		r2->v = *(vp + tp->v2);
		if (RotAverageNclip3(&r0->v, &r1->v, &r2->v,
		    &r0->sxy, &r1->sxy, &r2->sxy, 
		    &p, &sz, &flag) > 0) {
	  		ReadSZfifo3(&r0->sz, &r1->sz, &r2->sz);
	  		divp->ot = (u_long *)(ap->tagp->org + 
				((sz - ap->offset) >> ap->shift));
	  		divp->rgbc.cd = tp->code0;
			divp->clut = tp->clut;
			divp->tpage = tp->tpage;
			*(u_long *)(&r0->c) = *(u_long *)(&tp->r0);
			*(u_long *)(&r1->c) = *(u_long *)(&tp->r1);
			*(u_long *)(&r2->c) = *(u_long *)(&tp->r2);
			*(u_long *)r0->uv = *(u_long *)(&tp->uv0);
			*(u_long *)r1->uv = *(u_long *)(&tp->uv1);
			*(u_long *)r2->uv = *(u_long *)(&tp->uv2);
	  		pp =(POLY_GT3 *)RCpolyGT3A(pp, divp, lv, crv);		
		}
    	}
	GsOUT_PACKET_P = (PACKET *)pp;
  	return(ap->primp+1);
}
