/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_0004014d		/* */

	

	.text


/************************************************************************
* Preset Gouraud/Texture Triangle with no Lighting     			*
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

GsU_0004014d:

	.set    noreorder

	lw	a3,0(a0)		/* a3: sp->primp */
	lw	t9,4(a0)		/* t9: otp; */
	lw	t8,0(a3)		/* *(primp) */
	lw	t7,8(a0)		/* shift */
	lhu	v1,PSDIDX		/* current ID */
	addu	a3,a3,4			/* skip size, num */
	lw	a2,0(a3)		/* prim idx: *(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* n */

	beq	v1,0,NTG3_0		/* if(PSDIDX==0) goto NTG3_0; */
	nop
	li	v1,40	

NTG3_0:
	beq	t8,0,NTG3_3		/* if(n==0) goto NTG3_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	lw	t0,4(t9)		/* t0: otp->org */
	lw	t1,12(a0)		/* t1: otp->offset */

	addu	t9,a3,v1		/* t9: pkt_base */
	lw	a1,24(a0)		/* a1: vertop */

	lw	t2,76(a3)		/* t2: primtop->v0 */
	lw	t4,80(a3)		/* t4: primtop->v1,v2 */
	srl	t2,t2,16		/* t2: primtop->v0 */
	srl	t3,t4,16		/* t3: primtop->v2 */
	sll	t4,t4,16		/* t4: primtop->v1 */
	sll	t2,t2,3			/* t2: v0*8 */
	srl	t4,t4,13		/* t4: v1*8 */
	sll	t3,t3,3			/* t3: v2*8 */

	addu	t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v1) */
	addu	t3,t3,a1		/* t3: (vertop+primtop->v2) */

NTG3_1:
	lwc2	C2_VXY0,(t2)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t2)

	lwc2	C2_VXY1,(t4)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t4)

	lwc2	C2_VXY2,(t3)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t3)
	nop

	nop
	/****/
        RTPT				
	/****/

	addu	a3,a3,84		/* primtop++ */

	lw	t2,76(a3)		/* t2: primtop->v0 */
	lw	t4,80(a3)		/* t4: primtop->v1,v2 */
	srl	t2,t2,16		/* t2: primtop->v0 */
	srl	t3,t4,16		/* t3: primtop->v2 */
	sll	t4,t4,16		/* t4: primtop->v1 */
	sll	t2,t2,3			/* t2: v0*8 */
	srl	t4,t4,13		/* t4: v1*8 */
	sll	t3,t3,3			/* t3: v2*8 */

	addu	t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v1) */
	addu	t3,t3,a1		/* t3: (vertop+primtop->v2) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,NTG3_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/


	mfc2	v0,C2_MAC0		/* opz read */
	nop
#ifdef NORMAL_FLIP
	bge	v0,0,NTG3_2		/* Normal clip if MAC0(OPZ)>=0 (FLIP) */
#else
	ble	v0,0,NTG3_2		/* Normal clip if MAC0(OPZ)<=0 */
#endif
	nop


	/****/
	AVSZ3				
	/****/

	mfc2	t6,C2_OTZ		/* t6: otz */
	nop

	subu 	t6,t6,t1		/* sz = sz(otz) - otp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - otp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(table[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */

	li	v0,0x9000000		/* (sizeof(POLY_GT3)/4-1)<<24) */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_GT3)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

        swc2    C2_SXY0,8(t9)		/* sxy0 -->8(pkt_base) */
        swc2    C2_SXY1,20(t9)		/* sxy1 -->20(pkt_base) */
	swc2    C2_SXY2,32(t9) 		/* sxy2 -->32(pkt_base) */


NTG3_2:
	addu	t9,t9,84		/* pkt_base++; */
	subu	t8,t8,1
	bne	t8,0,NTG3_1		/* if(primtop != primtop+n) goto NTG3_1 */
	nop

NTG3_3:
	lw	v0,0(a0)		/* sp->primp */
	nop
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
