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
 * Shared Gouraud Quad -> Shared Flat Texture Quad (GsUSG4)
 * for HMD-ENV (1D-ENV for Shared G4)
 * 1997/10/16 (doubtful!!!!!!!!!!!!!!!!!!!!!!!!!!)
 */
u_long *GsU_06001114(GsARGUNIT *sp)
{
	register POLY_FT4	*pp;
	register HMD_P_G4	*tp;
	register int		i, j;
	GsARGUNIT_SHARED	*ap = (GsARGUNIT_SHARED *)sp;
	GsWORKUNIT		*wu = ap->vertop2;
	SVECTOR 		*cv = ap->nortop2;
	MATRIX			m0;
	long			sz;
	int			num;
	u_long			*tag;
	/* added variable */
	VECTOR lv;
	GsARGUNIT_ENVMAP	*ep = (GsARGUNIT_ENVMAP *)sp;
	u_short			tpage; /* to scratch */
	u_char			material2;

	pp = (POLY_FT4 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	tpage = GetTPage(ep->p00, 0, ((u_short *)(ep->tex1))[4],
	                 ((u_short *)(ep->tex1))[5]);
	material2 = ep->p02;

	ap->primp++;
	tp = (HMD_P_G4 *)(ap->primtop+(*(ap->primp)));

	for (i = 0; i < num; i++, tp++) {
		if ((*(u_long *)&(wu[tp->v0].otz) == 0xffffffff) ||
		    (*(u_long *)&(wu[tp->v1].otz) == 0xffffffff) ||
		    (*(u_long *)&(wu[tp->v2].otz) == 0xffffffff) ||
		    (*(u_long *)&(wu[tp->v3].otz) == 0xffffffff)) {
			continue;
		}
		*(u_long *)&(pp->x0) = *(u_long *)&(wu[tp->v0].vec);
		*(u_long *)&(pp->x1) = *(u_long *)&(wu[tp->v1].vec);
		*(u_long *)&(pp->x2) = *(u_long *)&(wu[tp->v2].vec);
		*(u_long *)&(pp->x3) = *(u_long *)&(wu[tp->v3].vec);

		gte_ldsxy3(*(long *)&(pp->x0), *(long *)&(pp->x1), 
			   *(long *)&(pp->x2));
		gte_nclip();
		gte_stopz(&sz);
		if (sz > 0)
			goto NOCLIP4;

		gte_ldsxy0(*(long *)&(pp->x3));
		gte_nclip();
		gte_stopz(&sz);
		if (sz > 0)
			continue;
		
		gte_avsz3();
		gte_stszotz(&sz);

		tag = (u_long *)((u_long *)ap->tagp->org + 
				((sz - ap->offset) >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x06000000;
		*tag = (u_long) pp & 0x00ffffff;
		pp++;
		continue;

NOCLIP4:
#if 0
/*		setcode(pp, 0x38); */
		m0.m[0][0] = tp->r0; m0.m[0][1] = 0; m0.m[0][2] = 0;
		m0.m[1][0] = 0; m0.m[1][1] = tp->g0; m0.m[1][2] = 0;
		m0.m[2][0] = 0; m0.m[2][1] = 0; m0.m[2][2] = tp->b0; 
		gte_SetRotMatrix(&m0);

		for (j = 0; j < 4; j++) {
			u_short tmpn = *((u_short *)&(tp->n0)+(j<<1));
			SVECTOR	rgb;
			gte_ldsv(&cv[tmpn]);
			gte_rtir();
			gte_stsv(&rgb);
/*			*((u_char *)&(pp->r0)+(j<<3)) = lim(rgb.vx,255); */
/*			*((u_char *)&(pp->g0)+(j<<3)) = lim(rgb.vy,255); */
/*			*((u_char *)&(pp->b0)+(j<<3)) = lim(rgb.vz,255); */
		}
#endif

		gte_ldsz4(wu[tp->v0].otz, wu[tp->v1].otz, wu[tp->v2].otz, 
			wu[tp->v3].otz);
		gte_avsz4();
		gte_stszotz(&sz);

		setPolyFT4(pp);
		setRGB0(pp,0x80,0x80,0x80);	/* default brightness */
		pp->tpage = tpage;

		/* UV calcurate */
		gte_ApplyRotMatrix(&cv[*((u_short *)&(tp->n0))], &lv);
		pp->u0 = ((int)lv.vy/32+128)&0xff;
		pp->v0 = material2;

		gte_ApplyRotMatrix(&cv[*((u_short *)&(tp->n1))], &lv);
		pp->u1 = ((int)lv.vy/32+128)&0xff;
		pp->v1 = material2;

		gte_ApplyRotMatrix(&cv[*((u_short *)&(tp->n2))], &lv);
		pp->u2 = ((int)lv.vy/32+128)&0xff;
		pp->v2 = material2;

		gte_ApplyRotMatrix(&cv[*((u_short *)&(tp->n3))], &lv);
		pp->u3 = ((int)lv.vy/32+128)&0xff;
		pp->v3 = material2;

		tag = (u_long *)((u_long *)ap->tagp->org + 
				((sz - ap->offset) >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x09000000;
		*tag = (u_long) pp & 0x00ffffff;
		pp++;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
