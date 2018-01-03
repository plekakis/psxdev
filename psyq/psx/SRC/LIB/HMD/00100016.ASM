/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

#ifdef SEMI_TRANS
	.globl	GsU_00300016		/* */
#else /* SEMI_TRANS */
	.globl	GsU_00100016		/* */
#endif /* SEMI_TRANS */

	

	.text


/************************************************************************
* (Semi-Trans) Back Facing Colored Gouraud Quad with Lighting
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

#ifdef SEMI_TRANS
GsU_00300016:
#else /* SEMI_TRANS */
GsU_00100016:
#endif /* SEMI_TRANS */

	.set    noreorder

	lw	a3,0(a0)		/* sp->primp */
	lw	t9,4(a0)		/* sp->tagp */
	lw	t8,0(a3)		/* *(sp->primp) */
	lw	t7,8(a0)		/* sp->shift */
	addu	a3,a3,4			/* skip size,num */
	lw	a2,0(a3)		/* prim idx:*(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* t8: *(sp->primp)>>16=num */
	beq	t8,0,CG4L_3		/* if(n==0) goto CG4L_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	lw	t0,4(t9)		/* t0: otp->org */
	lw	t1,12(a0)		/* t1: sp->offset */
	addu	v0,a0,16		/* t9: pkt_base */
	lw	t9,0(v0)
	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

	sw	s0,16(a0)		/* save s0 register */

CG4L_1:
	lw	t2,16(a3)		/* t2: primtop->v0,n0 */
	lw	t3,20(a3)		/* t3: primtop->v1,n1 */
	lw	t4,24(a3)		/* t4: primtop->v2,n2 */
	srl	t5,t2,16		/* t5: v0 * 8 */
	sll	t5,t5,3
	addu	t5,t5,a1		/* t5: (vertop+primtop->v0) */

	lwc2	C2_VXY0,(t5)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t5)

	srl	t5,t3,16		/* t5: v1 * 8 */
	sll	t5,t5,3
	addu	t5,t5,a1		/* t5: (vertop+primtop->v1) */
	lwc2	C2_VXY1,(t5)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t5)

	srl	t5,t4,16		/* t5: v2 * 8 */
	sll	t5,t5,3
	addu	t5,t5,a1		/* t5: (vertop+primtop->v2) */
	lwc2	C2_VXY2,(t5)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t5)
	sll	t2,t2,16		/* t2: (n0 & 0x0000ffff) * 8 */

	nop
	/****/
        RTPT				
	/****/

	lw	t5,28(a3)		/* t5: primtop->v3,n3 */

	srl	t2,t2,13		/* t2: (n0 & 0x0000ffff) * 8 [contd] */
	addu	t2,t2,a2		/* t2: (nortop+primtop->n0) */
	sll	t3,t3,16		/* t3: (n1 & 0x0000ffff) * 8 */
	srl	t3,t3,13
	addu	t3,t3,a2		/* t3: (nortop+primtop->n1) */
	sll	t4,t4,16		/* t4: (n2 & 0x0000ffff) * 8 */
	srl	t4,t4,13
	addu	t4,t4,a2		/* t4: (nortop+primtop->n2) */

	srl	v1,t5,16		/* v1: v3 * 8 */
	sll	v1,v1,3
	addu	v1,v1,a1		/* v1: (vertop+primtop->v3) */

	sll	t5,t5,16		/* t5: (n3 & 0x0000ffff) * 8 */
	srl	t5,t5,13
	addu	t5,t5,a2		/* t5: (nortop+primtop->n3) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,CG4L_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/

	lwc2	C2_VXY0,(v1)		/* Vector 0 <--(vertop+primtop->v3) */
	lwc2	C2_VZ0,4(v1)

	mfc2	s0,C2_MAC0		/* opz read */

    	swc2    C2_SXY0,8(t9)		/* sxy0 -->8(pkt_base) */
    	swc2    C2_SXY1,16(t9)		/* sxy1 -->16(pkt_base) */
	swc2    C2_SXY2,24(t9) 		/* sxy2 -->24(pkt_base) */
	nop

	nop
	/****/
        RTPS				
	/****/

	lw	v1,12(a3)		/* R3,G3,B3,cd <-- primtop->R3G3B3cd */
	nop
	mtc2	v1,C2_RGB		

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,CG4L_2		/* clip if flag <0 */
	nop

	/****/
	AVSZ4				
	/****/

	bge	s0,0,BACKF_0		/* if MAC0(OPZ)>=0 */
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
	mtc2	t6,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	mtc2	v0,C2_VZ0
	j	BACKF_1
	nop

BACKF_0:
	lwc2	C2_VXY0,(t5)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t5)

BACKF_1:
	mfc2	t6,C2_OTZ		/* t6: otz */
	nop
	nop
	/****/
        NCCS				
	/****/
	
	swc2	C2_RGB2,28(t9)		/* R3,G3,B3,cd -->28(pkt_base) */

	bge	s0,0,BACKF_2		/* if MAC0(OPZ)>=0 */
	nop

	lw	v1,0(t2)		/* back facing */
	lw	v0,4(t2)
	negu	v1,v1
	sll	t5,v1,16
	negu	t5,t5
	srl	t5,t5,16
	and	v1,v1,0xffff0000
	or	v1,v1,t5
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	v1,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	mtc2	v0,C2_VZ0
	j	BACKF_3
	nop

BACKF_2:
	lwc2	C2_VXY0,(t2)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t2)
BACKF_3:
	lw	v0,0(a3)		/* R0,G0,B0,cd <-- primtop->R0G0B0cd */
	nop
#ifdef SEMI_TRANS
	or	v0,v0,0x02000000	/* add ABE bit */
#endif /* SEMI_TRANS */
	mtc2	v0,C2_RGB		
	nop
	nop

	/****/
        NCCS				
	/****/
	
	subu 	t6,t6,t1		/* sz = sz(otz) - sp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - sp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(tagp->org[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */

	li	v0,0x8000000		/* (sizeof(POLY_G4)/4-1)<<24) */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_G4)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	nop
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

	lw	v0,4(a3)		/* R1,G1,B1,cd <-- primtop->R1G1B1cd */
	swc2	C2_RGB2,4(t9)		/* R0,G0,B0,cd -->4(pkt_base) */

	bge	s0,0,BACKF_4		/* if MAC0(OPZ)>=0 */
	nop

	lw	t6,0(t3)		/* back facing */
	lw	v0,4(t3)
	negu	t6,t6
	sll	t5,t6,16
	negu	t5,t5
	srl	t5,t5,16
	and	t6,t6,0xffff0000
	or	t6,t6,t5
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t6,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	mtc2	v0,C2_VZ0
	j	BACKF_5
	nop

BACKF_4:
	lwc2	C2_VXY0,(t3)		/* NX1,NY1,NZ1 <--(nortop+primtop->n1) */
	lwc2	C2_VZ0,4(t3)
BACKF_5:
	mtc2	v0,C2_RGB		
	nop
	nop
	/****/
        NCCS				
	/****/
	
	lw	v0,8(a3)		/* R2,G2,B2,cd <-- primtop->R2G2B2cd */

	bge	s0,0,BACKF_6		/* if MAC0(OPZ)>=0 */
	nop

	lw	t6,0(t4)		/* back facing */
	lw	v0,4(t4)
	negu	t6,t6
	sll	t5,t6,16
	negu	t5,t5
	srl	t5,t5,16
	and	t6,t6,0xffff0000
	or	t6,t6,t5
	sll	v0,v0,16
	negu	v0,v0
	srl	v0,v0,16
	mtc2	t6,C2_VXY0		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	mtc2	v0,C2_VZ0
	j	BACKF_7
	nop

BACKF_6:
	lwc2	C2_VXY0,(t4)		/* NX2,NY2,NZ2 <--(nortop+primtop->n2) */
	lwc2	C2_VZ0,4(t4)
BACKF_7:
	swc2	C2_RGB2,12(t9)		/* R1,G1,B1,cd -->12(pkt_base) */
	mtc2	v0,C2_RGB		
	nop
	nop
	/****/
        NCCS				
	/****/
	
	swc2    C2_SXY2,32(t9) 		/* sxy2 -->32(pkt_base) */
	swc2	C2_RGB2,20(t9)		/* R2,G2,B2,cd -->20(pkt_base) */

	addu	t9,t9,36		/* pkt_base++; */

CG4L_2:
	addu	a3,a3,32		/* primtop++ */

	subu	t8,t8,1
	bne	t8,0,CG4L_1		/* if(primtop != primtop+n) goto CG4L_1 */
	nop

CG4L_3:
	lw	s0,16(a0)		/* restore s0 register */
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

	.set    reorder
    	j       ra			/* return(primtop) */
