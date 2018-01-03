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
 * Light primitive (FIX)
 */
u_long *GsU_07020200(GsARGUNIT *sp)
{
	GsARGUNIT_LIGHT		*ap = (GsARGUNIT_LIGHT *)sp;
	HMD_P_LIGHT		*cp = (HMD_P_LIGHT *)
					(ap->lparam + (*(ap->primp+1) >> 16));
	u_long			id = *(ap->primp+1) & 0xffff;
	MATRIX			*lvmat = &GsLIGHTWSMATRIX;
	MATRIX			lcmat, lwmat;
	int			len;
	VECTOR			vec, tmp;

	GsGetLwUnit(ap->coord, &lwmat);
	tmp.vx = cp->vpx - cp->vrx;
	tmp.vy = cp->vpy - cp->vry;
	tmp.vz = cp->vpz - cp->vrz;
	ApplyMatrixLV(&lwmat, &tmp, &vec);

	len = SquareRoot0((vec.vx*vec.vx) + (vec.vy*vec.vy) + (vec.vz*vec.vz));
	if (len == 0) {
		goto END;
	}

	gte_ReadColorMatrix(&lcmat);

	lvmat->m[id][0] = vec.vx * ONE / len;
	lvmat->m[id][1] = vec.vy * ONE / len;
	lvmat->m[id][2] = vec.vz * ONE / len;
	lcmat.m[0][id] = (cp->r * ONE) >> 8;
	lcmat.m[1][id] = (cp->g * ONE) >> 8;
	lcmat.m[2][id] = (cp->b * ONE) >> 8;
	
	gte_SetColorMatrix(&lcmat);

END:
	return(ap->primp+2);
}
