/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 
 * PolyGT4 packet sorting routine	(Normal Light, mipmapping)
 *
 * 1995,1996 (C) Sony Computer Entertainment Inc. 
 *
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <inline_c.h>
#include <gtemac.h>
#include <libgpu.h>
#include "libgs.h"
#include <asm.h>

#if 0

/* Using libgte functions version */

PACKET *FastTG4Lmip(op, vp, np, pk, n, shift, ot)
	TMD_P_TG4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register POLY_GT4 *si;
	register int i;
	register	long *tag;
	long otz, flg, p, opz;
	CVECTOR	rgb0;

	si = (POLY_GT4 *) pk;
	setPolyGT4(si);
	setRGB0(si, 0x80, 0x80, 0x80);
	rgb0 = *(CVECTOR *)&si->r0;

	for (i = 0; i < n; i++, op++) {
		/* coordinate transformation, perspective transformation */
		opz = RotAverageNclip4((SVECTOR *)&vp[op->v0], (SVECTOR *)&vp[op->v1],
				(SVECTOR *)&vp[op->v2], (SVECTOR *)&vp[op->v3],
				(long *)&si->x0, (long *)&si->x1, (long *)&si->x2,
				(long *)&si->x3, &p, &otz, &flg);
		if (flg & 0x80000000)
			continue;
		if (opz <= 0)
			continue;
		*(u_long *) & si->u0 = *(u_long *) & op->tu0;
		*(u_long *) & si->u1 = *(u_long *) & op->tu1;
		*(u_long *) & si->u2 = *(u_long *) & op->tu2;
		*(u_long *) & si->u3 = *(u_long *) & op->tu3;

		/* mipmapping */
		if(otz > 8000){
			si->u0 >>= 2;
			si->u1 >>= 2;
			si->u2 >>= 2;
			si->u3 >>= 2;
			si->v0 >>= 2;
			si->v1 >>= 2;
			si->v2 >>= 2;
			si->v3 >>= 2;
		}else
		if(otz > 4000){
			si->u0 >>= 1;
			si->u1 >>= 1;
			si->u2 >>= 1;
			si->u3 >>= 1;
			si->v0 >>= 1;
			si->v1 >>= 1;
			si->v2 >>= 1;
			si->v3 >>= 1;
		}

		/* find normal colors from normal vectors */
		NormalColorCol3((SVECTOR *)&np[op->n0], (SVECTOR *)&np[op->n1],
				(SVECTOR *)&np[op->n2], &rgb0,
				(CVECTOR *)&si->r0, (CVECTOR *)&si->r1, (CVECTOR *)&si->r2);
		NormalColorCol((SVECTOR *)&np[op->n3], &rgb0, (CVECTOR *)&si->r3);

		/* set packet to ot */
		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (*tag & 0x00ffffff) | 0x0c000000;
		*tag = (u_long) si & 0x00ffffff;

		si++;
	}
	return (PACKET *) si;
}
#endif

#if 1

/* Using DMPSX functions version */

PACKET *FastTG4Lmip(op, vp, np, pk, n, shift, ot)
	TMD_P_TG4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register POLY_GT4 *si;
	register int i;
	register	long *tag;
	register	long vtmp;
	long otz, flg;

	si = (POLY_GT4 *) pk;
	setPolyGT4(si);
	setRGB0(si, 0x80, 0x80, 0x80);
	gte_ldrgb(&si->r0);

	for (i = 0; i < n; i++, op++) {
		gte_ldv3(&vp[op->v0], &vp[op->v1], &vp[op->v2]);
		gte_rtpt();			/* RotTransPers3 */
		*(u_long *) & si->u0 = *(u_long *) & op->tu0;
		*(u_long *) & si->u1 = *(u_long *) & op->tu1;
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_nclip();		/* NormalClip */
		*(u_long *) & si->u2 = *(u_long *) & op->tu2;
		gte_stopz(&otz);	/* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_gt4((u_long *) si);
		gte_ldv0(&vp[op->v3]);
		gte_rtps();			/* RotTransPers */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *) & si->x3);
		gte_avsz4();
		*(u_long *) & si->u3 = *(u_long *) & op->tu3;
		gte_stotz(&otz);

		/* mipmapping */
		if(otz > 8000){
			/* texture to the scale of one sixteenth, addressed (0,8) */
			*(u_short *)&si->u0 = (*(short *)&si->u0 >> 2) & 0xff3f;
			*(u_short *)&si->u1 = (*(short *)&si->u1 >> 2) & 0xff3f;
			*(u_short *)&si->u2 = (*(short *)&si->u2 >> 2) & 0xff3f;
			*(u_short *)&si->u3 = (*(short *)&si->u3 >> 2) & 0xff3f;
		}else
		if(otz > 4000){
			/* texture to the scale of one fourth, addressed (0,16) */
			*(u_short *)&si->u0 = (*(u_short *)&si->u0 >> 1) & 0xff7f;
			*(u_short *)&si->u1 = (*(u_short *)&si->u1 >> 1) & 0xff7f;
			*(u_short *)&si->u2 = (*(u_short *)&si->u2 >> 1) & 0xff7f;
			*(u_short *)&si->u3 = (*(u_short *)&si->u3 >> 1) & 0xff7f;
		}
		/* texture of original scale, addressed (0, 32) */

		gte_ldv3(&np[op->n0], &np[op->n1], &np[op->n2]);	/* lighting */
		gte_ncct();			/* NormalColorCol3 */
		gte_strgb3(&si->r0, &si->r1, &si->r2);
		gte_ldv0(&np[op->n3]);
		gte_nccs();			/* NormalColorCol */
		/* set packet to ot */
		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (*tag & 0x00ffffff) | 0x0c000000;
		*tag = (u_long) si & 0x00ffffff;
		gte_strgb(&si->r3);
		si++;
	}
	return (PACKET *) si;
}
#endif


/* Using DMPSX function version */
PACKET *FastG4L(op, vp, np, pk, n, shift, ot)
	TMD_P_G4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register	POLY_G4 *si;
	register	int i;
	register	long *tag;
	long	flg, otz;

	si = (POLY_G4 *) pk;

	for (i = 0; i < n; i++, op++) {
		gte_ldv3(&vp[op->v0], &vp[op->v1], &vp[op->v2]);
		gte_rtpt();			/* RotTransPers3 */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		gte_stopz(&otz);	/* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_g4((u_long *) si);
		gte_ldv0(&vp[op->v3]);
		gte_rtps();			/* RotTransPers */
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *) & si->x3);
		gte_avsz4();
		gte_stotz(&otz);

		gte_ldrgb(&op->r0);
		gte_ldv3(&np[op->n0], &np[op->n1], &np[op->n2]);	/* lighting */
		gte_ncct();			/* NormalColorCol3 */
		gte_strgb3_g4((u_long *)si);
		gte_ldv0(&np[op->n3]);
		gte_nccs();			/* NormalColorCol */
		/* set packet to ot */
		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x08000000);
		*tag = (u_long) si & 0x00ffffff;
		gte_strgb(&si->r3);
		si++;
	}
	return (PACKET *) si;
}


