/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_00300015		/* */

	

	.text


/************************************************************************
* Back Facing Semi-Trans Gouraud/Texture Quad with Lighting
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

GsU_00300015:

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
	beq	t8,0,TG4L_3		/* if(n==0) goto TG4L_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	li	v0,0x3e808080		/* v0: RGBcd */
	lw	t0,4(t9)		/* t0: table */
	mtc2    v0,C2_RGB 		/* R,G,B,cd <-- v0: GPU_COD_TG4 */

	lw	t1,12(a0)		/* t1: otp->offset */

	addu	v0,a0,16
	lw	t9,0(v0)		/* t9: pkt_base */

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

TG4L_1:

	lw	t2,12(a3)		/* t2: primtop->v0,uv3 */
	lw	t3,16(a3)		/* t3: primtop->v1,n1 */
	lw	t4,20(a3)		/* t4: primtop->v2,n2 */
TG4L_1L:
	srl	t5,t2,16		/* t5: v0 */
	sll	t5,t5,3			/* t5: v0*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v0) */
	lwc2	C2_VXY0,(t5)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t5)

	srl	t5,t3,16		/* t5: v1 */
	sll	t5,t5,3			/* t5: v1*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v1) */
	lwc2	C2_VXY1,(t5)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t5)

	srl	t5,t4,16		/* t5: v2 */
	sll	t5,t5,3			/* t5: v2*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v2) */
	lwc2	C2_VXY2,(t5)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t5)
	nop
	nop

	/****/
        RTPT				
	/****/

	lw	t2,8(a3)		/* t2: primtop->n0,uv2 */
	lw	t5,24(a3)		/* t5: primtop->v3,n3 */

	srl	t2,t2,16		/* t2: n0<<16 */
	sll	t2,t2,3			/* t2: n0*8 */
	addu	t2,t2,a2		/* t2: (nortop+primtop->n0) */
	sll	t3,t3,16		/* t3: n1<<16 */
	srl	t3,t3,13		/* t3: n1*8 */
	addu	t3,t3,a2		/* t3: (nortop+primtop->n1) */
	sll	t4,t4,16		/* t4: n2<<16 */
	srl	t4,t4,13		/* t4: n2*8 */
	addu	t4,t4,a2		/* t4: (nortop+primtop->n2) */

	srl	v1,t5,16		/* v1: v3 */
	sll	v1,v1,3			/* v1: v3*8 */
	addu	v1,v1,a1		/* v1: (vertop+primtop->v3) */

	sll	t5,t5,16		/* t5: n3<<16 */
	srl	t5,t5,13		/* t5: n3*8 */
	addu	t5,t5,a2		/* t5: (nortop+primtop->n3) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,TG4L_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/

	lwc2	C2_VXY0,(v1)		/* Vector 0 <--(vertop+primtop->v3) */
	lwc2	C2_VZ0,4(v1)

	mfc2	v1,C2_MAC0		/* opz read */
        swc2    C2_SXY0,8(t9)		/* sxy0 -->8(pkt_base) */
        swc2    C2_SXY1,20(t9)		/* sxy1 -->20(pkt_base) */
	swc2    C2_SXY2,32(t9) 		/* sxy2 -->32(pkt_base) */
	nop
	nop

	/****/
        RTPS				
	/****/

	bge	v1,0,BACKF_0		/* if MAC0(OPZ)<=0 */
	nop

	lw	t6,0(t3)		/* back facing */
	lw	v0,4(t3)
	negu	t6,t6
	sll	t3,t6,16
	negu	t3,t3
	srl	t3,t3,16
	and	t6,t6,0xffff0000
	or	t6,t6,t3
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t6,C2_VXY1		/* NX1,NY1,NZ1 <--(nortop+primtop->n1) */
	mtc2	v0,C2_VZ1

	lw	t6,0(t4)
	lw	v0,4(t4)
	negu	t6,t6
	sll	t4,t6,16
	negu	t4,t4
	srl	t4,t4,16
	and	t6,t6,0xffff0000
	or	t6,t6,t4
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t6,C2_VXY2		/* NX2,NY2,NZ2 <--(nortop+primtop->n2) */
	mtc2	v0,C2_VZ2
	j	BACKF_1
	nop

BACKF_0:
	lwc2	C2_VXY1,(t3)		/* NX1,NY1,NZ1 <--(nortop+primtop->n1) */
	lwc2	C2_VZ1,4(t3)
	lwc2	C2_VXY2,(t4)		/* NX2,NY2,NZ2 <--(nortop+primtop->n2) */
	lwc2	C2_VZ2,4(t4)

BACKF_1:

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,TG4L_2		/* clip if flag <0 */
	nop

	/****/
	AVSZ4				
	/****/

	bge	v1,0,BACKF_2		/* if MAC0(OPZ)<=0 */
	nop

	lw	t6,0(t5)		/* back facing */
	lw	v0,4(t5)
	negu	t6,t6
	sll	t5,t6,16
	negu	t5,t5
	srl	t5,t5,16
	and	t6,t6,0xffff0000
	or	t6,t6,t5
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t6,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n3) */
	mtc2	v0,C2_VZ0
	j	BACKF_3
	nop

BACKF_2:
	lwc2	C2_VXY0,(t5)		/* NX0,NY0,NZ0 <--(nortop+primtop->n3) */
	lwc2	C2_VZ0,4(t5)

BACKF_3:
	mfc2	t6,C2_OTZ		/* t6: otz */
	nop
	nop

	/****/
        NCCS				
	/****/

	lw	v0,0(a3)		/* primtop->uv0 */
	lw	t3,4(a3)		/* primtop->uv1 */
	lw	t4,8(a3)		/* primtop->uv2 */
	lw	t5,12(a3)		/* primtop->uv3 */
	sw    	v0,12(t9) 		/* uv0 -->12(pkt_base) */
	sw    	t3,24(t9) 		/* uv1 -->24(pkt_base) */
	sw    	t4,36(t9) 		/* uv2 -->36(pkt_base) */
	sw    	t5,48(t9) 		/* uv3 -->48(pkt_base) */

	swc2	C2_RGB2,40(t9)		/* R2,G2,B2,cd(R3,G3,B3,cd)->40(pkt_base) */

	bge	v1,0,BACKF_4		/* if MAC0(OPZ)>=0 */
	nop

	lw	t3,0(t2)		/* back facing */
	lw	v0,4(t2)
	negu	t3,t3
	sll	t5,t3,16
	negu	t5,t5
	srl	t5,t5,16
	and	t3,t3,0xffff0000
	or	t3,t3,t5
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t3,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	mtc2	v0,C2_VZ0
	j	BACKF_5
	nop

BACKF_4:
	lwc2	C2_VXY0,(t2)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t2)
	nop
	nop
BACKF_5:

	/****/
        NCCT				
	/****/
	
	subu 	t6,t6,t1		/* sz = sz(otz) - otp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - otp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(table[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */
	
	li	v0,0xc000000		/* (sizeof(POLY_GT4)/4-1)<<24) */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_GT4)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) of v0 */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

	addu	a3,a3,28		/* primtop++ */
	lw	t2,12(a3)		/* t2: primtop->v0,n0 */
	lw	t3,16(a3)		/* t3: primtop->v1,n1 */
	lw	t4,20(a3)		/* t4: primtop->v2,n2 */

	swc2    C2_SXY2,44(t9) 		/* sxy3 -->44(pkt_base) */
	swc2	C2_RGB0,4(t9)		/* R0,G0,B0,cd -->4(pkt_base) */
	swc2	C2_RGB1,16(t9)		/* R1,G1,B1,cd -->16(pkt_base) */
	swc2	C2_RGB2,28(t9)		/* R2,G2,B2,cd -->28(pkt_base) */

TG4L_2L:

	addu	t9,t9,52		/* pkt_base++; */
	subu	t8,t8,1
	bne	t8,0,TG4L_1L		/* if(primtop != primtop+n) goto TG4L_1 */
	nop
	b 	TG4L_3			
	nop
TG4L_2:

	addu	a3,a3,28		/* primtop++ */
	subu	t8,t8,1

	bne	t8,0,TG4L_1		/* if(primtop != primtop+n) goto TG4L_1 */
	nop
TG4L_3:
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
