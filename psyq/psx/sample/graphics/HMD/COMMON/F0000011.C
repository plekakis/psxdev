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
	u_char	tu2, tv2; u_short	pad;
	u_char	tu3, tv3; u_short	n0;
	u_short	v0, v1;
	u_short	v2, v3;
}	HMD_P_FT4;

/*
 * Flat Texture Triangle (GsUFT4)
 */
u_long *GsU_f0000011(GsARGUNIT *sp)
{
	POLY_FT4		*pp;
	LINE_F2			*pf2;
	HMD_P_FT4 		*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i;
	SVECTOR			vv[2];
	CVECTOR			code = {0x80, 0x80, 0x80, 0x2c};
	u_long			rgbcd = 0x400000ff;

	pp = (POLY_FT4 *)ap->out_packetp;
	num = *(ap->primp)>>16;

	ap->primp++;
	tp = (HMD_P_FT4 *)(ap->primtop+(*(ap->primp)));

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

		gte_ldrgb(&code);
		gte_ldv0(&(ap->nortop)[tp->n0]);	/* lighting */
		gte_nccs();

		gte_stsxy3_ft4((u_long *)pp);
		gte_avsz3();

		gte_ldv0(&(ap->vertop)[tp->v3]);	/* lighting */
		gte_rtps();

		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *)&pp->x3);

		gte_stotz(&otz);

		tag = (u_long *)((u_long *)ap->tagp->org + 
			(otz >> ap->shift));
		*(u_long *)pp = (*tag & 0x00ffffff) | 0x09000000;
		*tag = (u_long)pp & 0x00ffffff;
		gte_strgb(&pp->r0);

		vv[0].vx = ((ap->vertop)[tp->v0].vx + (ap->vertop)[tp->v1].vx +
			(ap->vertop)[tp->v2].vx + (ap->vertop)[tp->v3].vx) / 4;
		vv[0].vy = ((ap->vertop)[tp->v0].vy + (ap->vertop)[tp->v1].vy +
			(ap->vertop)[tp->v2].vy + (ap->vertop)[tp->v3].vy) / 4;
		vv[0].vz = ((ap->vertop)[tp->v0].vz + (ap->vertop)[tp->v1].vz +
			(ap->vertop)[tp->v2].vz + (ap->vertop)[tp->v3].vz) / 4;

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
		pp = (POLY_FT4 *)pf2;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
