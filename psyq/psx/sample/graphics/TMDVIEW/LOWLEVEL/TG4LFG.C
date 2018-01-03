/* $PSLibId: Run-time Library Release 4.4$ */
/* 
 * PolyGT4 packet sorting routine (FOG on)
 *
 * 1995 (C) Sony Computer Entertainment Inc. 
 *
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <inline_c.h>
#include <gtemac.h>
#include <libgpu.h>
#include <libgs.h>
#include <asm.h>

#if 0

/* Using libgte functions version */

PACKET *FastTG4LFG(op, vp, np, pk, n, shift, ot)
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

		/* find normal colors from normal vectors */
		NormalColorDpq3((SVECTOR *)&np[op->n0], (SVECTOR *)&np[op->n1],
				(SVECTOR *)&np[op->n2], &rgb0, p,
				(CVECTOR *)&si->r0, (CVECTOR *)&si->r1, (CVECTOR *)&si->r2);
		NormalColorDpq((SVECTOR *)&np[op->n3], &rgb0, p, (CVECTOR *)&si->r3);

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

PACKET *FastTG4LFG(op, vp, np, pk, n, shift, ot)
	TMD_P_TG4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register POLY_GT4 *si;
	register int i;
	register	long *tag;
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

		gte_ldv3(&np[op->n0], &np[op->n1], &np[op->n2]);	/* lighting */
		gte_ncdt();			/* NormalColorDpq3 */
		gte_strgb3(&si->r0, &si->r1, &si->r2);
		gte_ldv0(&np[op->n3]);
		gte_ncds();			/* NormalColorDpq */
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

