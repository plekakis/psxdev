/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_00040154		/* */

	

	.text


/************************************************************************
* Preset Gouraud Quad with no Lighting						*
*
*	a0: (GsARGUNIT *)sp
*
*	typedef struct {
*		u_long	*primp;		// 0(a0)
*		GsOT 	*tagp;		// 4(a0)
*		int	shift;		// 8(a0)
*		int	offset;		// 12(a0)
*		PACKET	*out_packetp;	// 16(a0)
*		u_long	*primtop;	// 20(a0)
*		SVECTOR	*vertop;	// 24(a0)
*		SVECTOR	*nortop;	// 28(a0)
*	}	GsARGUNIT;
*	num:				// (*primtop) >> 16
*************************************************************************/

GsU_00040154:

	.set    noreorder

	lw	a3,0(a0)		/* a3: sp->primp */
	lw	t9,4(a0)		/* t9: otp; */
	lw	t8,0(a3)		/* *(sp->primtop) */
	lhu	v1,PSDIDX		/* current ID */
	addu	a3,a3,4			/* skip num,size */
	srl	t8,t8,16		/* n */
	lw	a2,0(a3)		/* prim idx: *(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	lw	t7,8(a0)		/* shift */

	beq	v1,0,NG4_0		/* if(PSDIDX==0) goto NG4_0; */
	nop
	li	v1,36

NG4_0:
	beq	t8,0,NG4_3		/* if(n==0) goto NG4_3: */
	addu 	a3,a1,a2		/* primtop: sp->primtop+idx */

	lw	t0,4(t9)		/* t0: table */
	lw	t1,12(a0)		/* t1: otp->offset */

	addu	t9,a3,v1		/* t9: pkt_base */

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

	lw	t2,72(a3)		/* t2: primtop->v0,v1 */
	lhu	t4,76(a3)		/* t4: primtop->v2 */
	srl	t3,t2,16		/* t3: v1 */
	sll	t2,t2,16		/* t2: v0 */
	sll	t3,t3,3			/* v1*8 */
	srl	t2,t2,13		/* v0*8 */
	sll	t4,t4,3			/* v2*8 */

	addu	t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu	t3,t3,a1		/* t3: (vertop+primtop->v1) */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v2) */

NG4_1:
	lwc2	C2_VXY0,(t2)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t2)

	lwc2	C2_VXY1,(t3)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t3)

	lwc2	C2_VXY2,(t4)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t4)
	nop

	nop
	/****/
        RTPT				
	/****/

	lhu	t5,78(a3)		/* t5: primtop->v3 */
	nop
	sll	t5,t5,3			/* v3*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v3) */

	addu	a3,a3,80		/* primtop++ */

	lw	t2,72(a3)		/* t2: primtop->v0,v1 */
	lhu	t4,76(a3)		/* t4: primtop->v2 */
	srl	t3,t2,16		/* t3: v1 */
	sll	t2,t2,16		/* t2: v0 */
	sll	t3,t3,3			/* v1*8 */
	srl	t2,t2,13		/* v0*8 */
	sll	t4,t4,3			/* v2*8 */

	addu	t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu	t3,t3,a1		/* t3: (vertop+primtop->v1) */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v2) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,NG4_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/

	lwc2	C2_VXY0,(t5)		/* Vector 0 <--(vertop+primtop->v3) */
	lwc2	C2_VZ0,4(t5)

	mfc2	v1,C2_MAC0		/* opz read */
	nop
	ble	v1,0,NG4_2		/* Normal clip if MAC0(OPZ)<=0 */
	nop
        swc2    C2_SXY0,8(t9)		/* sxy0 -->8(pkt_base) */
        swc2    C2_SXY1,16(t9)		/* sxy1 -->20(pkt_base) */
	swc2    C2_SXY2,24(t9) 		/* sxy2 -->32(pkt_base) */
	nop

	nop
	/****/
        RTPS				
	/****/

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,NG4_2		/* clip if flag <0 */
	nop

	/****/
	AVSZ4				
	/****/

	mfc2	t6,C2_OTZ		/* t6: otz */
	nop

	subu 	t6,t6,t1		/* sz = sz(otz) - otp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - otp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(table[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */
	
	li	v0,0x8000000		/* (sizeof(POLY_G4)/4-1)<<24) */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_G4)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */
	
	swc2    C2_SXY2,32(t9) 		/* sxy3 -->32(pkt_base) */

NG4_2:
	addu	t9,t9,80		/* pkt_base++; */
	subu	t8,t8,1

	bne	t8,0,NG4_1		/* if(primtop != primtop+n) goto NG4_1 */
	nop
NG4_3:
	lw	v0,0(a0)		/* sp->primp */
	nop
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
