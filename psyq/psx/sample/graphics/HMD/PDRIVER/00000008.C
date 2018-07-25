/*
 * $PSLibId: Run-time Library Release 4.3$
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

typedef struct {
	u_char	r0, g0, b0, code;
	u_short	n0, v0;
	u_short	v1, v2;
}	HMD_P_F3;

/*
 * Flat Triangle (GsUF3)
 */
u_long *GsU_00000008(GsARGUNIT *sp)
{
	POLY_F3			*pp;
	HMD_P_F3 		*tp;
	GsARGUNIT_NORMAL	*ap = (GsARGUNIT_NORMAL *)sp;
	int			num;
	long			flg, otz;
	u_long			*tag;
	int			i;

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

		pp++;
	}
	GsOUT_PACKET_P = (PACKET *)pp;
	return(ap->primp+1);
}
