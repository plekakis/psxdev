/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for all of category 7 equipment primitive types.

	Copyright (C) 1998  Sony Computer Entertainment Inc.
	All rights Reserved.

	Ver 1.00	Feb 25, 1997	By N. Yoshioka
	Ver 1.01	Mar 02, 1997	By N. Yoshioka
		- Made changes for final camera spec.
	Ver 1.02	Mar 03, 1997	By N. Yoshioka
		- For lights.
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
scan_equip(u_long type)
{
	/* CTG 7: EQUIP i.e. Camera */

	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x07);

	switch (type) {

	/* DEV_ID(SCE)|CTG(7)|DRV(0x00 )|PRIM_TYPE(0x0100 ); */
	case 0x07000100:	func = GsU_07000100;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x01 )|PRIM_TYPE(0x0100 ); */
	case 0x07010100:	func = GsU_07010100;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x02 )|PRIM_TYPE(0x0100 ); */
	case 0x07020100:	func = GsU_07020100;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x03 )|PRIM_TYPE(0x0100 ); */
	case 0x07030100:	func = GsU_07030100;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x00 )|PRIM_TYPE(0x0200 ); */
	case 0x07000200:	func = GsU_07000200;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x01 )|PRIM_TYPE(0x0200 ); */
	case 0x07010200:	func = GsU_07010200;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x02 )|PRIM_TYPE(0x0200 ); */
	case 0x07020200:	func = GsU_07020200;	break;
	/* DEV_ID(SCE)|CTG(7)|DRV(0x03 )|PRIM_TYPE(0x0200 ); */
	case 0x07030200:	func = GsU_07030200;	break;

	default:
		printf("unsupported EQUIPMENT primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
