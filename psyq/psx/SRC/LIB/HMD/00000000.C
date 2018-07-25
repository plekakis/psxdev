/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


/*#define DEBUG	/**/

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>

/*
 * unknown type(skip)
 */
u_long *GsU_00000000(GsARGUNIT *sp)
{
	int size;

	size = *(sp->primp) & 0xffff;
	return(sp->primp+size);
}
