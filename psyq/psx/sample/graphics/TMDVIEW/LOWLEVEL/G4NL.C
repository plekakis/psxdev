/* $PSLibId: Run-time Library Release 4.4$ */
/* 
 *	PolyG4 packet sorting routine (No Light)
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

PACKET *FastG4NL(op, vp, np, pk, n, shift, ot)
	TMD_P_G4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	register	POLY_G4 *si;
	register	int i;
	register	long *tag;
	long	flg, otz, p, opz;

	si = (POLY_G4 *) pk;

	for (i = 0; i < n; i++, op++) {

		/* coordinate transformation, perspective transformation */
		opz = RotAverageNclip4((SVECTOR *)&vp[op->v0], (SVECTOR *)&vp[op->v1],
				(SVECTOR *)&vp[op->v2], (SVECTOR *)&vp[op->v3],
				(long *)&si->x0, (long *)&si->x1, (long *)&si->x2,
				(long *)&si->x3, &p, &otz, &flg);
		if(flg & 0x80000000)
			continue;
		if(opz <= 0)
			continue;

		/* set original colors to the packet */
		*(u_long *)&si->r0 = *(u_long *)&op->r0;
		*(u_long *)&si->r1 = *(u_long *)&op->r0;
		*(u_long *)&si->r2 = *(u_long *)&op->r0;
		*(u_long *)&si->r3 = *(u_long *)&op->r0;

		/* set packet to ot */
		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x08000000);
		*tag = (u_long) si & 0x00ffffff;
		si++;
	}
	return (PACKET *) si;
}

#endif

#if 1

/* Using DMPSX functions version */

PACKET *FastG4NL(op, vp, np, pk, n, shift, ot)
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
		*(u_long *)&si->r0 = *(u_long *)&op->r0;
		*(u_long *)&si->r1 = *(u_long *)&op->r0;
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();		/* NormalClip */
		*(u_long *)&si->r2 = *(u_long *)&op->r0;
		gte_stopz(&otz);	/* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_g4((u_long *) si);
		gte_ldv0(&vp[op->v3]);
		gte_rtps();			/* RotTransPers */
		*(u_long *)&si->r3 = *(u_long *)&op->r0;
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *) & si->x3);
		gte_avsz4();
		gte_stotz(&otz);

		/* set packet to ot */
		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x08000000);
		*tag = (u_long) si & 0x00ffffff;

		si++;
	}
	return (PACKET *) si;
}

#endif
