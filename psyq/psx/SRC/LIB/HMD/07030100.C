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

#define C_LIMIT		16

typedef struct {
	long		proj;
	long		rot;
	long		vpx,vpy,vpz;
	long		vrx,vry,vrz;
} HMD_P_CAMERA;

extern MATRIX  GsWSMATRIX;	/* view scope matrix including scale screen */
extern MATRIX  GsWSMATRIX_ORG;	/* view scope matrix orignal */

static void scale_view_param(VECTOR *in0, VECTOR *in1, GsRVIEWUNIT *out);
static long select_max_param(VECTOR *in0, VECTOR *in1);
static long len_param(long number);

/*
 * Camera primitive (AIM)
 */
u_long *GsU_07030100(GsARGUNIT *sp)
{
	GsARGUNIT_CAMERA	*ap = (GsARGUNIT_CAMERA *)sp;
	HMD_P_CAMERA		*cp = (HMD_P_CAMERA *)ap->cparam;
	GsRVIEWUNIT		view2;
	long    		x0, y0, z0;
	short			rx, ry;
	MATRIX			mat, mat2, mat3, mat4;
	short			sx, sy, cx, cy;
	VECTOR  		tmp, tmp2;
	int     		f1, f2;
	int			i, j;
	
	GsGetLwUnit((GsCOORDUNIT *)ap->coord, &mat4);
	GsGetLwUnit((GsCOORDUNIT *)ap->rcoord, &mat2);
	TransposeMatrix(&mat4, &mat3);

	ApplyMatrixLV(&mat2, (VECTOR *)&cp->vrx, &tmp);
	tmp.vx += (mat2.t[0] - mat4.t[0]);
	tmp.vy += (mat2.t[1] - mat4.t[1]);
	tmp.vz += (mat2.t[2] - mat4.t[2]);
	ApplyMatrixLV(&mat3, &tmp, &tmp2);

	GsWSMATRIX = GsIDMATRIX2;
	gte_rotate_z_matrix(&GsWSMATRIX, -cp->rot);
	
	scale_view_param((VECTOR *)&cp->vpx, &tmp2, &view2);
	
	f1 = SquareRoot0((view2.vrx - view2.vpx) * (view2.vrx - view2.vpx) +
			 (view2.vry - view2.vpy) * (view2.vry - view2.vpy) +
			 (view2.vrz - view2.vpz) * (view2.vrz - view2.vpz));

	if (f1 == 0)
		goto END;

	f2 = (view2.vpy - view2.vry);

	sx = -(f2 * ONE / f1);
	f2 = SquareRoot0((view2.vrx - view2.vpx) * (view2.vrx - view2.vpx) + 
			 (view2.vrz - view2.vpz) * (view2.vrz - view2.vpz));
	cx = f2 * ONE / f1;
	Gssub_make_matrix(&mat, sx, cx, 'x');
	MulMatrix(&GsWSMATRIX, &mat);

	if (f2 > C_LIMIT) {
		f1 = f2;
		f2 = (view2.vrx - view2.vpx);
		sy = -(f2 * 4096 / f1);
		f2 = (view2.vrz - view2.vpz);
		cy = f2 * ONE / f1;
		Gssub_make_matrix(&mat, sy, cy, 'y');
		MulMatrix(&GsWSMATRIX, &mat);
	}
	tmp.vx = -(long) cp->vpx;
	tmp.vy = -(long) cp->vpy;
	tmp.vz = -(long) cp->vpz;

	ApplyMatrixLV(&GsWSMATRIX, &tmp, (VECTOR *) & GsWSMATRIX.t[0]);

	ApplyMatrixLV(&mat3, (VECTOR *) & mat4.t[0], &tmp);
	mat3.t[0] = -tmp.vx;
	mat3.t[1] = -tmp.vy;
	mat3.t[2] = -tmp.vz;
	GsMulCoord2(&GsWSMATRIX, &mat3);
	GsWSMATRIX = mat3;
	GsWSMATRIX_ORG = GsWSMATRIX;	/* copying orignal */

END:
	return(ap->primp+1);
}

static void scale_view_param(VECTOR *in0, VECTOR *in1, GsRVIEWUNIT *out)
{
	int     	len;

	len = len_param(select_max_param(in0, in1));
	/*
	 * FntPrint("len = %d\n",len); 
	 */
	if (len > 15) {
		len -= 15;

		out->vpx = in0->vx >> len;
		out->vpy = in0->vy >> len;
		out->vpz = in0->vz >> len;
		out->vrx = in1->vx >> len;
		out->vry = in1->vy >> len;
		out->vrz = in1->vz >> len;
	} else {
		out->vpx = in0->vx;
		out->vpy = in0->vy;
		out->vpz = in0->vz;
		out->vrx = in1->vx;
		out->vry = in1->vy;
		out->vrz = in1->vz;
	}
}

static long select_max_param(VECTOR *in0, VECTOR *in1)
{
	long    ret;

	ret = (in0->vx > 0 ? in0->vx : -in0->vx);
	ret = (ret > (in0->vy > 0 ? in0->vy : -in0->vy) ? ret : 
			(in0->vy > 0 ? in0->vy : -in0->vy));
	ret = (ret > (in0->vz > 0 ? in0->vz : -in0->vz) ? ret : 
			(in0->vz > 0 ? in0->vz : -in0->vz));
	ret = (ret > (in1->vx > 0 ? in1->vx : -in1->vx) ? ret : 
			(in1->vx > 0 ? in1->vx : -in1->vx));
	ret = (ret > (in1->vy > 0 ? in1->vy : -in1->vy) ? ret : 
			(in1->vy > 0 ? in1->vy : -in1->vy));
	ret = (ret > (in1->vz > 0 ? in1->vz : -in1->vz) ? ret : 
			(in1->vz > 0 ? in1->vz : -in1->vz));
	return ret;
}

static long len_param(long number)
{
	long    i;
	/*
	 * FntPrint("number = %x\n",number); 
	 */
	for (i = 0; number > 0; i++)
		number >>= 1;

	return i;
}
