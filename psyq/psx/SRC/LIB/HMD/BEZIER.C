/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>

void setBezierCof(long cp0,long cp1,long cp2,long cp3,short *co)
{
    co[0] = -cp0 + 3*(cp1-cp2) + cp3;
    co[1] = 3*(cp0+cp2) - 6*cp1;
    co[2] = 3*(-cp0+cp1);
    co[3] = cp0;
}
