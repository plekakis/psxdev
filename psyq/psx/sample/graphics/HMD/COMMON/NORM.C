/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for normview

	Copyright (C) 1997,1998  Sony Computer Entertainment Inc.
	All rights Reserved.

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
scan_norm(u_long type)
{
	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x00);

	switch (type&0x0000ffff) {
	case 0x00000000:
#ifdef DEBUG
		printf("DEBUG:pointer is already set\n");
#endif /* DEBUG */
		func = GsU_00000000;
		break;
	case 0x00000008:	func = GsU_f0000008;	break;
	case 0x00000009:	func = GsU_f0000009;	break;
	case 0x0000000c:	func = GsU_f000000c;	break;
	case 0x0000000d:	func = GsU_f000000d;	break;
	case 0x00000010:	func = GsU_f0000010;	break;
	case 0x00000011:	func = GsU_f0000011;	break;
	case 0x00000014:	func = GsU_f0000014;	break;
	case 0x00000015:	func = GsU_f0000015;	break;
	default:
		printf("unsupported primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
