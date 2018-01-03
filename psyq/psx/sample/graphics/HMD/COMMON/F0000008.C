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
	u_short	v1, v2;
}	HMD_P_F3;

/*
 * Flat Triangle (GsUF3)
 */
u_long *GsU_f0000008(GsARGUNIT *sp)
{
	POLY_F3			*pp;
	LINE_F2			*pf2;
	HMD_P_F3 		*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i;
	SVECTOR			vv[2];
	u_long			rgbcd = 0x400000ff;

	pp = (POLY_F3 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_F3 *)(ap->primtop+(*(ap->primp)));

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

		gte_ldrgb((CVECTOR *)&tp->r0);
		gte_ldv0(&(ap->nortop)[tp->n0]);	/* lighting */
		gte_nccs();

		gte_stsxy3_f3((u_long *)pp);
		gte_avsz3();

		gte_stotz(&otz);

		tag = (u_long *)((u_long *)ap->tagp->org + 
			(otz >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x04000000;
		*tag = (u_long)pp & 0x00ffffff;
		gte_strgb(&pp->r0);

		vv[0].vx = ((ap->vertop)[tp->v0].vx + (ap->vertop)[tp->v1].vx +
			(ap->vertop)[tp->v2].vx) / 3;
		vv[0].vy = ((ap->vertop)[tp->v0].vy + (ap->vertop)[tp->v1].vy +
			(ap->vertop)[tp->v2].vy) / 3;
		vv[0].vz = ((ap->vertop)[tp->v0].vz + (ap->vertop)[tp->v1].vz +
			(ap->vertop)[tp->v2].vz) / 3;

		pp++;

		pf2 = (LINE_F2 *)pp;

		gte_ldv0(&vv[0]);
		gte_rtps();
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_stsxy(&pf2->x0);

		vv[1].vx = ((ap->nortop)[tp->n0].vx>>NORM_SHFT) + vv[0].vx;
		vv[1].vy = ((ap->nortop)[tp->n0].vy>>NORM_SHFT) + vv[0].vy;
		vv[1].vz = ((ap->nortop)[tp->n0].vz>>NORM_SHFT) + vv[0].vz;

		gte_ldv0(&vv[1]);
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
		pp = (POLY_F3 *)pf2;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
