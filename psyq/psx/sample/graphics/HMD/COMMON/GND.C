/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for all of category 5 ground primitive types.

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
scan_gnd(u_long type)
{
	/* CTG 5: GROUND */

	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x05);

	switch (type) {
	/* DEV_ID(SCE)|CTG(CTG_GND)|DRV(0)|PRIM_TYPE(0); */
	case 0x05000000:	func = GsU_05000000;	break;
	/* DEV_ID(SCE)|CTG(CTG_GND)|DRV(0)|PRIM_TYPE(TME); */
	case 0x05000001:	func = GsU_05000001;	break;
	default:
		printf("unsupported GROUND primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
