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
	u_char	tu0, tv0; u_short	cba;
	u_char	tu1, tv1; u_short	tsb;
	u_char	tu2, tv2; u_short	n0;
	u_char	tu3, tv3; u_short	v0;
	u_short	n1, v1;
	u_short	n2, v2;
	u_short	n3, v3;
}	HMD_P_GT4;

/*
 * Flat Triangle (GsUGT4)
 */
u_long *GsU_f0000015(GsARGUNIT *sp)
{
	POLY_GT4		*pp;
	LINE_F2			*pf2;
	HMD_P_GT4 		*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i, j;
	SVECTOR			vv[1];
	CVECTOR			code = {0x80, 0x80, 0x80, 0x3c};
	u_long			rgbcd = 0x400000ff;
	short			nn;

	pp = (POLY_GT4 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_GT4 *)(ap->primtop+(*(ap->primp)));

	for (i = 0; i < num; i++, tp++) {
		gte_ldv3(&(ap->vertop)[tp->v0], 
			 &(ap->vertop)[tp->v1],
			 &(ap->vertop)[tp->v2]);
		gte_rtpt();
		*(u_long *)&pp->u0 = *(u_long *)&tp->tu0;
		*(u_long *)&pp->u1 = *(u_long *)&tp->tu1;
		*(u_long *)&pp->u2 = *(u_long *)&tp->tu2;
		*(u_long *)&pp->u3 = *(u_long *)&tp->tu3;

		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_nclip();
		gte_stopz(&otz);    /* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_gt4((u_long *)pp);

		gte_ldv0(&(ap->vertop)[tp->v3]);
		gte_rtps();

		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *)&pp->x3);

		gte_avsz4();
		gte_stotz(&otz);


		gte_ldrgb(&code);
		gte_ldv3(&(ap->nortop)[tp->n0], 
			 &(ap->nortop)[tp->n1],
			 &(ap->nortop)[tp->n2]);
		gte_ncct();

		gte_strgb3_gt4((u_long *)pp);

		gte_ldv0(&(ap->nortop)[tp->n3]);
		gte_nccs();

		gte_strgb((u_long *)&pp->r3);

		tag = (u_long *)((u_long *)ap->tagp->org + 
			(otz >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x0c000000;
		*tag = (u_long)pp & 0x00ffffff;

		pp++;

		pf2 = (LINE_F2 *)pp;

		for (j = 0; j < 4; j++) {
			*(u_long *)&pf2->x0 = *(u_long *)(&(pp-1)->x0+j*6);
			if (j == 0) {
				nn = tp->n0;
			} else {
				nn = *(&(tp->n1)+(j-1)*2);
			}
			vv[0].vx = ((ap->nortop)[nn].vx>>NORM_SHFT) + 
				(ap->vertop)[*(&(tp->v0)+j*2)].vx;
			vv[0].vy = ((ap->nortop)[nn].vy>>NORM_SHFT) +
				(ap->vertop)[*(&(tp->v0)+j*2)].vy;
			vv[0].vz = ((ap->nortop)[nn].vz>>NORM_SHFT) +
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
		pp = (POLY_GT4 *)pf2;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
