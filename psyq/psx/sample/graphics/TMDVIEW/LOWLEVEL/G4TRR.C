/* $PSLibId: Run-time Library Release 4.4$ */
/* 
 *	PolyG4 packet sorting routine (TransRotPers version)
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

PACKET *FastG4LTrr(op, vp, np, pk, n, shift, ot)
	TMD_P_G4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	POLY_G4	*si;
	int	i;
	long	*tag;
	long	flg, otz, p, opz;
	SVECTOR	svp[4];
	VECTOR	invvp, dbvp/* = {ONE, ONE, ONE}*/;
	MATRIX	orgmtr, invmtr, id2mtr, lvpmp;

	si = (POLY_G4 *) pk;

	dbvp.vx = dbvp.vz = ONE;
	dbvp.vy = ONE*HWD0*3/VWD0/4;
	ReadRotMatrix(&orgmtr);
	ReadRotMatrix(&id2mtr);
	ScaleMatrix(&id2mtr, &dbvp);
	TransposeMatrix(&id2mtr, &invmtr);
	ScaleMatrix(&invmtr, &dbvp);
	ApplyMatrixLV(&invmtr, (VECTOR *)orgmtr.t, (VECTOR *)id2mtr.t);
	SetRotMatrix(&orgmtr);
	SetTransMatrix(&id2mtr);

	for (i = 0; i < n; i++, op++) {

		otz = TransRotPers3((SVECTOR *)&vp[op->v0], (SVECTOR *)&vp[op->v1],
			(SVECTOR *)&vp[op->v2], (long *)&si->x0, (long *)&si->x1,
			(long *)&si->x2, &p, &flg);
		if(flg & 0x80000000)
			continue;

		opz = NormalClip(*((long *)&si->x0), *((long *)&si->x1),
			*((long *)&si->x2));
		if(opz <= 0)
			continue;

		TransRotPers((SVECTOR *)&vp[op->v3], (long *)&si->x3, &p, &flg);
		if(flg & 0x80000000)
			continue;

		NormalColorCol3((SVECTOR *)&np[op->n0], (SVECTOR *)&np[op->n1],
			(SVECTOR *)&np[op->n2], (CVECTOR *)&op->r0,
			(CVECTOR *)&si->r0, (CVECTOR *)&si->r1, (CVECTOR *)&si->r2);
		NormalColorCol((SVECTOR *)&np[op->n3], (CVECTOR *)&op->r0,
			(CVECTOR *)&si->r3);

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x08000000);
		*tag = (u_long) si & 0x00ffffff;
		si++;
	}
	SetTransMatrix(&orgmtr);
	return (PACKET *) si;
}
#endif

#if 1

/* Using DMPSX functions version */

PACKET *FastG4LTrr(op, vp, np, pk, n, shift, ot)
	TMD_P_G4 *op;
	VERT   *vp, *np;
	PACKET *pk;
	int     n, shift;
	GsOT   *ot;
{
	POLY_G4	*si;
	int	i;
	long	*tag;
	long	flg, otz;
	SVECTOR	svp[4];
	VECTOR	lvp, invvp, dbvp = {ONE, ONE, ONE};
	MATRIX	orgmtr, invmtr, id2mtr;

	si = (POLY_G4 *) pk;

	gte_ReadRotMatrix(&orgmtr);
	gte_ReadRotMatrix(&id2mtr);
	dbvp.vy = ONE*HWD0*3/VWD0/4;
	ScaleMatrix(&id2mtr, &dbvp);
	TransposeMatrix(&id2mtr, &invmtr);
	ScaleMatrix(&invmtr, &dbvp);
	ApplyMatrixLV(&invmtr, (VECTOR *)orgmtr.t, &invvp);
	gte_SetRotMatrix(&orgmtr);
	lvp.vx = lvp.vy = lvp.vz = 0;
	gte_SetTransVector(&lvp);

	for (i = 0; i < n; i++, op++) {
		svp[0].vx = vp[op->v0].vx + (short)(invvp.vx);
		svp[1].vx = vp[op->v1].vx + (short)(invvp.vx);
		svp[2].vx = vp[op->v2].vx + (short)(invvp.vx);
		svp[0].vy = vp[op->v0].vy + (short)(invvp.vy);
		svp[1].vy = vp[op->v1].vy + (short)(invvp.vy);
		svp[2].vy = vp[op->v2].vy + (short)(invvp.vy);
		svp[0].vz = vp[op->v0].vz + (short)(invvp.vz);
		svp[1].vz = vp[op->v1].vz + (short)(invvp.vz);
		svp[2].vz = vp[op->v2].vz + (short)(invvp.vz);
		svp[3].vx = vp[op->v3].vx + (short)(invvp.vx);
		svp[3].vy = vp[op->v3].vy + (short)(invvp.vy);
		svp[3].vz = vp[op->v3].vz + (short)(invvp.vz);

		gte_ldv3(&svp[0], &svp[1], &svp[2]);
		gte_rtpt();
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;
		gte_nclip();
		gte_stopz(&otz);	/* back clip */
		if (otz <= 0)
			continue;

		gte_stsxy3_g4((u_long *) si);
		gte_ldv0(&svp[3]);
		gte_rtps();
		gte_stflg(&flg);
		if (flg & 0x80000000)
			continue;

		gte_stsxy((u_long *) & si->x3);
		gte_avsz4();
		gte_stotz(&otz);

		gte_ldrgb(&op->r0);
		gte_ldv3(&np[op->n0], &np[op->n1], &np[op->n2]);	/* lighting */
		gte_ncct();
		gte_strgb3(&si->r0, &si->r1, &si->r2);
		gte_ldv0(&np[op->n3]);
		gte_nccs();
		gte_strgb(&si->r3);

		tag = (u_long *) (ot->org + (otz >> shift));
		*(u_long *) si = (u_long)((*tag & 0x00ffffff) | 0x08000000);
		*tag = (u_long) si & 0x00ffffff;

		si++;
	}
	gte_SetTransVector(orgmtr.t);
	return (PACKET *) si;
}
#endif

