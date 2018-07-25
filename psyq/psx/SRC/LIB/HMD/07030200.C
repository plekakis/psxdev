/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>

#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>

typedef struct {
	u_char		r, g, b, pad;
	long		vpx,vpy,vpz;
	long		vrx,vry,vrz;
} HMD_P_LIGHT;

/*
 * Light primitive (AIM)
 */
u_long *GsU_07030200(GsARGUNIT *sp)
{
	GsARGUNIT_LIGHT		*ap = (GsARGUNIT_LIGHT *)sp;
	HMD_P_LIGHT		*cp = (HMD_P_LIGHT *)
					(ap->lparam + (*(ap->primp+1) >> 16));
	u_long			id = *(ap->primp+1) & 0xffff;
	MATRIX			*lvmat = &GsLIGHTWSMATRIX;
	MATRIX			lcmat, mat, rmat;
	int			len;
	VECTOR			vec, rvec, lvec;

	GsGetLwUnit(ap->coord, &mat);
	GsGetLwUnit(ap->rcoord, &rmat);

	ApplyMatrixLV(&rmat, (VECTOR *)&cp->vrx, &rvec);
	rvec.vx += rmat.t[0];
	rvec.vy += rmat.t[1];
	rvec.vz += rmat.t[2];

	ApplyMatrixLV(&mat, (VECTOR *)&cp->vpx, &vec);
	vec.vx += mat.t[0];
	vec.vy += mat.t[1];
	vec.vz += mat.t[2];

	lvec.vx = vec.vx - rvec.vx;
	lvec.vy = vec.vy - rvec.vy;
	lvec.vz = vec.vz - rvec.vz;

	len = SquareRoot0(
		(lvec.vx*lvec.vx) + (lvec.vy*lvec.vy) + (lvec.vz*lvec.vz));
	if (len == 0) {
		goto END;
	}

	gte_ReadColorMatrix(&lcmat);

	lvmat->m[id][0] = lvec.vx * ONE / len;
	lvmat->m[id][1] = lvec.vy * ONE / len;
	lvmat->m[id][2] = lvec.vz * ONE / len;
	lcmat.m[0][id] = (cp->r * ONE) >> 8;
	lcmat.m[1][id] = (cp->g * ONE) >> 8;
	lcmat.m[2][id] = (cp->b * ONE) >> 8;
	
	gte_SetColorMatrix(&lcmat);

END:
	return(ap->primp+2);
}
