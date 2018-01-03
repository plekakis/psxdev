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

typedef struct {
	long		proj;
	long		rot;
	long		vpx,vpy,vpz;
	long		vrx,vry,vrz;
} HMD_P_CAMERA;

extern MATRIX  GsWSMATRIX;	/* view scope matrix including scale screen */
extern MATRIX  GsWSMATRIX_ORG;	/* view scope matrix orignal */

static void scale_view_param(HMD_P_CAMERA *in, GsRVIEWUNIT *out);
static long select_max_param(HMD_P_CAMERA *in);
static long len_param(long number);

/*
 * Camera primitive (WORLD)
 */
u_long *GsU_07010100(GsARGUNIT *sp)
{
	GsARGUNIT_CAMERA	*ap = (GsARGUNIT_CAMERA *)sp;
	HMD_P_CAMERA		*cp = (HMD_P_CAMERA *)ap->cparam;
	GsRVIEWUNIT		view2;
	long    		x0, y0, z0;
	short			rx, ry;
	MATRIX			mat, mat2, mat3;
	short			sx, sy, cx, cy;
	VECTOR  		tmp;
	int     		f1, f2;
	int			i, j;
	
	GsWSMATRIX = GsIDMATRIX2;
	gte_rotate_z_matrix(&GsWSMATRIX, -cp->rot);
	
	scale_view_param(cp, &view2);
	
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

	if (f2 != 0) {
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

END:
	return(ap->primp+1);
}

static void scale_view_param(HMD_P_CAMERA *in, GsRVIEWUNIT *out)
{
	int     	len;

	len = len_param(select_max_param(in));
	/*
	 * FntPrint("len = %d\n",len); 
	 */
	if (len > 15) {
		len -= 15;

		out->vpx = in->vpx >> len;
		out->vpy = in->vpy >> len;
		out->vpz = in->vpz >> len;
		out->vrx = in->vrx >> len;
		out->vry = in->vry >> len;
		out->vrz = in->vrz >> len;
	} else {
		out->vpx = in->vpx;
		out->vpy = in->vpy;
		out->vpz = in->vpz;
		out->vrx = in->vrx;
		out->vry = in->vry;
		out->vrz = in->vrz;
	}
}

static long select_max_param(HMD_P_CAMERA *in)
{
	long    ret;

	ret = (in->vpx > 0 ? in->vpx : -in->vpx);
	ret = (ret > (in->vpy > 0 ? in->vpy : -in->vpy) ? ret : 
			(in->vpy > 0 ? in->vpy : -in->vpy));
	ret = (ret > (in->vpz > 0 ? in->vpz : -in->vpz) ? ret : 
			(in->vpz > 0 ? in->vpz : -in->vpz));
	ret = (ret > (in->vrx > 0 ? in->vrx : -in->vrx) ? ret : 
			(in->vrx > 0 ? in->vrx : -in->vrx));
	ret = (ret > (in->vry > 0 ? in->vry : -in->vry) ? ret : 
			(in->vry > 0 ? in->vry : -in->vry));
	ret = (ret > (in->vrz > 0 ? in->vrz : -in->vrz) ? ret : 
			(in->vrz > 0 ? in->vrz : -in->vrz));
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
