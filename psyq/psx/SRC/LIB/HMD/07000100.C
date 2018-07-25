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

/*
 * Camera primitive (PROJECTION)
 */
u_long *GsU_07000100(GsARGUNIT *sp)
{
	GsARGUNIT_CAMERA	*ap = (GsARGUNIT_CAMERA *)sp;
	HMD_P_CAMERA		*cp = (HMD_P_CAMERA *)ap->cparam;

	SetGeomScreen(cp->proj);
	return(ap->primp+1);
}
