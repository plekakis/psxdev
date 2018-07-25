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
	u_char  r0, g0, b0, code;
	u_short n0, v0;
	u_short v1, v2;
	u_short v3, pad;
}       HMD_P_F4;

/*
 * Divide Flat Quad (GsUDF4)
 */
u_long *GsU_00010010(GsARGUNIT *sp)
{
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	POLY_F4			*pp;
	HMD_P_F4		*tp;
	DIVPOLYGON4		*divp = (DIVPOLYGON4 *)(&(ap->nortop)+1);
  	RVECTOR			*r0, *r1, *r2, *r3;
	SVECTOR			*vp = ap->vertop;
	SVECTOR			*np = ap->nortop;
  	int			i;
  	u_short			sz;
  	long			p, flag;
  	CRVECTOR4		*crv;
  	long			lv;
	int			num;

	pp = (POLY_F4 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_F4 *)(ap->primtop+(*(ap->primp)&0x00ffffff));

	divp->ndiv = *(ap->primp)>>24;
	divp->pih = HWD0;
	divp->piv = VWD0;

  	r0= &divp->r0;
  	r1= &divp->r1;
  	r2= &divp->r2;
  	r3= &divp->r3;
  	lv=0;
  	crv = divp->cr;
  	divp->cr[lv].r0 = r0;		
  	divp->cr[lv].r1 = r1;		
  	divp->cr[lv].r2 = r2;		
  	divp->cr[lv].r3 = r3;		
  
  	for (i = 0; i < num; i++, tp++) {
		r0->v = *(vp + tp->v0);
		r1->v = *(vp + tp->v1);
		r2->v = *(vp + tp->v2);
		r3->v = *(vp + tp->v3);
		if (RotAverageNclip4(&r0->v, &r1->v, &r2->v, &r3->v,
		    &r0->sxy, &r1->sxy, &r2->sxy, &r3->sxy,
		    &p, &sz, &flag) > 0) {
	  		ReadSZfifo4(&r0->sz, &r1->sz, &r2->sz, &r3->sz);
			NormalColorCol(np+tp->n0, &tp->r0, &divp->rgbc);
	  		divp->ot = (u_long *)(ap->tagp->org + 
				((sz - ap->offset) >> ap->shift));
	  		divp->rgbc.cd = tp->code;
	  		pp =(POLY_F4 *)RCpolyF4A(pp, divp, lv, crv);		
		}
    	}
	GsOUT_PACKET_P = (PACKET *)pp;
  	return(ap->primp+1);
}
