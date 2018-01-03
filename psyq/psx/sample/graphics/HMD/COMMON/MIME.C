/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for all of category 4 MIMe primitive types.

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
scan_mime(u_long type)
{
	/* CTG 4: MIMe */

	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x04);

	switch (type) {
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(JNT_AXES); */
	case 0x04010010:	func = GsU_04010010;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(JNT_RPY); */
	case 0x04010011:	func = GsU_04010011;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(RST|JNT_AXES); */
	case 0x04010018:	func = GsU_04010018;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(RST|JNT_RPY); */
	case 0x04010019:	func = GsU_04010019;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(VTX); */
	case 0x04010020:	func = GsU_04010020;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(NRM); */
	case 0x04010021:	func = GsU_04010021;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(RST|VTX); */
	case 0x04010028:	func = GsU_04010028;	break;
	/* DEV_ID(SCE)|CTG(CTG_MIMe)|DRV(MIMe_PRIM)|PRIM_TYPE(RST|NRM); */
	case 0x04010029:	func = GsU_04010029;	break;
	default:
		printf("unsupported MIMe primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
