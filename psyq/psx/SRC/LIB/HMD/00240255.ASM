/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

#ifdef BACK_FACE
	.globl	GsU_00340255		/* */
#else /* BACK_FACE */
	.globl	GsU_00240255		/* */
#endif /* BACK_FACE */

	

	.text


/************************************************************************
* (Back-Facing) Tile Semi-Trans Gouraud/Texture Quad with no Lighting
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

#ifdef BACK_FACE
GsU_00340255:
#else /* BACK_FACE */
GsU_00240255:
#endif /* BACK_FACE */

	.set    noreorder

	lw	a3,0(a0)		/* a3: sp->primp */
	lw	t9,4(a0)		/* t9: otp; */
	lw	t8,0(a3)		/* *(sp->primp) */
	lw	t7,8(a0)		/* shift */
	addu	a3,a3,4			/* skip num,size */
	lw	a2,0(a3)		/* prim idx: *(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* n */
	beq	t8,0,NTG4_3		/* if(n==0) goto NTG4_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	lw	t0,4(t9)		/* t0: table */
	lw	t1,12(a0)		/* t1: otp->offset */

	addu	v0,a0,16
	lw	t9,0(v0)		/* t9: pkt_base */

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

	lhu     t2,30(a3)		/* t2: primtop->v0 */
	lhu     t3,34(a3)	       	/* t3: primtop->v1 */
	lhu     t4,36(a3)	       	/* t4: primtop->v2 */

	sll     t2,t2,3	 	 	/* v0*8 */
	sll     t3,t3,3		 	/* v1*8 */
	sll     t4,t4,3		 	/* v2*8 */
	addu    t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu    t3,t3,a1		/* t3: (vertop+primtop->v1) */
	addu    t4,t4,a1		/* t4: (vertop+primtop->v2) */

NTG4_1:
        lwc2    C2_VXY0,(t2)            /* Vector 0 <--(vertop+primtop->v0) */
        lwc2    C2_VZ0,4(t2)

        lwc2    C2_VXY1,(t3)            /* Vector 1 <--(vertop+primtop->v1) */
        lwc2    C2_VZ1,4(t3)

        lwc2    C2_VXY2,(t4)            /* Vector 2 <--(vertop+primtop->v2) */
        lwc2    C2_VZ2,4(t4)
	nop
	nop

	/****/
        RTPT				
	/****/

        lhu     v1,38(a3)               /* v1: primtop->v3 */
	lw	v0,0(a3)		/* tw */
	lw	t2,4(a3)		/* primtop->rgb0 */
	lw	t3,8(a3)		/* primtop->rgb1 */
	or	t2,t2,0x02000000	/* add ABE bit */
	sw	v0,4(t9)		/* tw */
	sw	t2,8(t9)		/* rgb0 */
	sw	t3,20(t9)		/* rgb1 */
	lw	t2,12(a3)		/* primtop->rgb2 */
	lw	t3,16(a3)		/* primtop->rgb3 */
	sw	t2,32(t9)		/* rgb2 */
	sw	t3,44(t9)		/* rgb3 */
	sll	v1,v1,3			/* v3*8 */
	addu	v1,v1,a1		/* v1: (vertop+primtop->v3) */

	addu	a3,a3,40		/* primtop++ */

	lhu     t2,30(a3)	       	/* t2: primtop->v0 */
	lhu     t3,34(a3)	       	/* t3: primtop->v1 */
	lhu     t4,36(a3)	       	/* t4: primtop->v2 */

	sll     t2,t2,3	 	 	/* v0*8 */
	sll     t3,t3,3		 	/* v1*8 */
	sll     t4,t4,3		 	/* v2*8 */
	addu    t2,t2,a1		/* t2: (vertop+primtop->v0) */
	addu    t3,t3,a1		/* t3: (vertop+primtop->v1) */
	addu    t4,t4,a1		/* t4: (vertop+primtop->v2) */
	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,NTG4_2		/* clip if flag <0 */
	nop

#ifndef BACK_FACE
	/****/
        NCLIP				
	/****/
#endif /* !BACK_FACE */

	lwc2	C2_VXY0,(v1)		/* Vector 0 <--(vertop+primtop->v3) */
	lwc2	C2_VZ0,4(v1)

#ifndef BACK_FACE
	mfc2	v1,C2_MAC0		/* opz read */
	nop
	ble	v1,0,NTG4_2		/* Normal clip if MAC0(OPZ)<=0 */
	nop
#endif /* !BACK_FACE */
        swc2    C2_SXY0,12(t9)		/* sxy0 -->12(pkt_base) */
        swc2    C2_SXY1,24(t9)		/* sxy1 -->24(pkt_base) */
	swc2    C2_SXY2,36(t9) 		/* sxy2 -->36(pkt_base) */
	nop
	nop

	/****/
        RTPS				
	/****/

	subu	v1,a3,40		/* old primtop */
	lw	v0,20(v1)		/* primtop->uv0 */
	lw	t6,24(v1)		/* primtop->uv1 */
	sw    	v0,16(t9) 		/* uv0 -->16(pkt_base) */
	sw    	t6,28(t9) 		/* uv1 -->28(pkt_base) */
	lw	v0,28(v1)		/* primtop->uv2 */
	lw	t6,32(v1)		/* primtop->uv3 */
	sw    	v0,40(t9) 		/* uv2 -->40(pkt_base) */
	sw    	t6,52(t9) 		/* uv3 -->52(pkt_base) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,NTG4_2		/* clip if flag <0 */
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
	
	li	v0,0xe000000		/* (sizeof(POLY_GT4)/4-1)<<24)+2 */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_GT4)/4-1)<<24)+2 */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	.set noat
	li	AT,0xe2000000		/* tw reset */
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sw	AT,56(t9)		/* tw reset */
	.set at

	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) of v0 */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

	swc2    C2_SXY2,48(t9) 		/* sxy3 -->48(pkt_base) */

	addu	t9,t9,60		/* pkt_base++; */

NTG4_2:
	subu	t8,t8,1
	bne	t8,0,NTG4_1		/* if(primtop != primtop+n) goto NTG4_1 */
	nop

NTG4_3:
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
