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

void setBetaCof(long cp0,long cp1,long cp2,long cp3,short *co)
{
    co[0] = (-(ONE/6)*cp0 + (ONE/2)*cp1 - (ONE/2)*cp2 + (ONE/6)*cp3)>>12;
    co[1] =  ((ONE/2)*cp0 -     ONE*cp1 + (ONE/2)*cp2)>>12;
    co[2] = (-(ONE/2)*cp0               + (ONE/2)*cp2)>>12;
    co[3] =  ((ONE/6)*cp0+(ONE*2/3)*cp1 + (ONE/6)*cp2)>>12;
}
