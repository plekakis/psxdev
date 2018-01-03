/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
	Scanning routine for all of category 6 envmap primitive types.
	(This primitive type is beta version for now).

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
scan_envmap(u_long type)
{
	/* CTG 6: ENVMAP(beta) */

	GsUNIT_Funcp	func = NULL;

	assert((type >> 24) == 0x06);

	switch (type) {
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x0100 ); GsUSCAL2 */
	case 0x06000100:	func = GsU_06000100;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x100c ); GsUE1G3 */
	case 0x0600100c:	func = GsU_0600100c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x1014 ); GsUE1G4 */
	case 0x06001014:	func = GsU_06001014;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x110c ); GsUE1SG3 */
	case 0x0600110c:	func = GsU_0600110c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x1114 ); GsUE1SG4 */
	case 0x06001114:	func = GsU_06001114;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x200c ); GsUE2LG3 */
	case 0x0600200c:	func = GsU_0600200c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x2014 ); GsUE2LG4 */
	case 0x06002014:	func = GsU_06002014;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x300c ); GsUE2RG3 */
	case 0x0600300c:	func = GsU_0600300c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x3014 ); GsUE2RG4 */
	case 0x06003014:	func = GsU_06003014;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x400c ); GsUE2RLG3 */
	case 0x0600400c:	func = GsU_0600400c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x4014 ); GsUE2RLG4 */
	case 0x06004014:	func = GsU_06004014;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x500c ); GsUE2OLG3 */
	case 0x0600500c:	func = GsU_0600500c;	break;
	/* DEV_ID(SCE)|CTG(6)|DRV(0x00 )|PRIM_TYPE(0x5014 ); GsUE2OLG4 */
	case 0x06005014:	func = GsU_06005014;	break;
	default:
		printf("unsupported ENVMAP primitive 0x%08x.\n", type);
	}

	return (u_long)func;
}
