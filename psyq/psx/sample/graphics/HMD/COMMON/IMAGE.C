/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for all of category 0 image primitive types.

	Copyright (C) 1997,1998  Sony Computer Entertainment Inc.
	All rights Reserved.

	Ver 1.00	Dec 09, 1997	By N. Yoshioka
	Ver 1.10	Jan 12, 1998	By N. Yoshioka
*/

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libhmd.h>
#include <stdio.h>
#include <assert.h>
#include "scan.h"

u_long
scan_image(u_long type)
{
	/* CTG 2: IMAGE */

	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x02);

	switch (type) {
	/* DEV_ID(SCE)|CTG(CTG_IMAGE)|DRV(0)|PRIM_TYPE(NOCLUT); */
	case 0x02000000:	func = GsU_02000000;	break;
	/* DEV_ID(SCE)|CTG(CTG_IMAGE)|DRV(0)|PRIM_TYPE(WITHCLUT); */
	case 0x02000001:	func = GsU_02000001;	break;
	default:
		printf("unsupported IMAGE primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
