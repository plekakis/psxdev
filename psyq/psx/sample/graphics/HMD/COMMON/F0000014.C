/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/**--------------------------------------------------------------*
 **
 **      Copyright(C) 1997 Sony Computer Entertainment Inc.
 **      All rights reserved.
 **
 **--------------------------------------------------------------*/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>

#include "scan.h"

typedef struct {
	u_char	r0, g0, b0, code;
	u_short	n0, v0;
	u_short	n1, v1;
	u_short	n2, v2;
	u_short	n3, v3;
}	HMD_P_G4;

/*
 * Flat Triangle (GsUG4)
 */
u_long *GsU_f0000014(GsARGUNIT *sp)
{
	POLY_G4			*pp;
	LINE_F2			*pf2;
	HMD_P_G4 		*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i, j;
	SVECTOR			vv[1];
	u_long			rgbcd = 0x400000ff;

	pp = (POLY_G4 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_G4 *)(ap->primtop+(*(ap->primp)));

	for (i = 0; i < num; i++, tp++) {
		gte_ldv3(&(ap->vertop)[tp->v0], 
			 &(ap->vertop)[tp->v1],
			 &(ap->vertop)[tp->v2]);
		gte_rtpt();

		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_nclip();
		gte_stopz(&otz);    /* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_g4((u_long *)pp);

		gte_ldv0(&(ap->vertop)[tp->v3]);
		gte_rtps();

		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *)&pp->x3);

		gte_avsz4();
		gte_stotz(&otz);

		gte_ldrgb((CVECTOR *)&tp->r0);
		gte_ldv3(&(ap->nortop)[tp->n0], 
			 &(ap->nortop)[tp->n1],
			 &(ap->nortop)[tp->n2]);
		gte_ncct();
		gte_strgb3_g4((u_long *)pp);

		gte_ldv0(&(ap->nortop)[tp->n3]);
		gte_nccs();

		gte_strgb((u_long *)&pp->r3);

		tag = (u_long *)((u_long *)ap->tagp->org + 
			(otz >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x08000000;
		*tag = (u_long)pp & 0x00ffffff;

		pp++;

		pf2 = (LINE_F2 *)pp;

		for (j = 0; j < 4; j++) {
			*(u_long *)&pf2->x0 = *(u_long *)(&(pp-1)->x0+j*4);
	
			vv[0].vx = 
				((ap->nortop)[*(&(tp->n0)+j*2)].vx>>NORM_SHFT)+
				(ap->vertop)[*(&(tp->v0)+j*2)].vx;
			vv[0].vy = 
				((ap->nortop)[*(&(tp->n0)+j*2)].vy>>NORM_SHFT)+
				(ap->vertop)[*(&(tp->v0)+j*2)].vy;
			vv[0].vz = 
				((ap->nortop)[*(&(tp->n0)+j*2)].vz>>NORM_SHFT)+
				(ap->vertop)[*(&(tp->v0)+j*2)].vz;
	
			gte_ldv0(&vv[0]);
			gte_rtps();
			gte_stflg(&flg);
			if (flg & 0x80000000)
				continue;
			gte_stsxy(&pf2->x1);
	
			tag = (u_long *)((u_long *)ap->tagp->org + 
				(otz >> ap->shift));
			*(u_long *)pf2 = (*tag & 0x00ffffff) | 0x03000000;
			*tag = (u_long)pf2 & 0x00ffffff;
	
			*(u_long *)&pf2->r0 = rgbcd;
	
			pf2++;
		}
		pp = (POLY_G4 *)pf2;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
