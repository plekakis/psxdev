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
	.globl	GsU_01340055
#else /* BACK_FACE */
	.globl	GsU_01240055
#endif /* BACK_FACE */

	
	.text
	

/************************************************************************
* (Back-Facing) Semi-Trans Shared Non-light Flat Gouraud Quad (GsUSTPSNGT4)
*
*       a0: (GsARGUNIT_SHARED *)sp
*
*       typedef struct {
*               u_long          *primp;         // 0(a0)
*               GsOT            *tagp;          // 4(a0)
*               int             shift;          // 8(a0)
*               int             offset;         // 12(a0)
*               PACKET          *out_packetp;   // 16(a0)
*               u_long          *primtop;       // 20(a0)
*               SVECTOR         *vertop;        // 24(a0)
*               GsWORKUNIT      *vertop2;       // 28(a0)
*               SVECTOR         *nortop;        // 32(a0)
*               SVECTOR         *nortop2;       // 36(a0)
*       }       GsARGUNIT_SHARED;
*       num:                            // (*primtop) >> 16
*************************************************************************/

#ifdef BACK_FACE
GsU_01340055:
#else /* BACK_FACE */
GsU_01240055:
#endif /* BACK_FACE */

	.set	noreorder

	lw	t0,0(a0)		/* t0: primp */
	lw	t1,16(a0)		/* t1: out_packetp */
	lhu	t2,2(t0)		/* t2: num */
	lw	t3,4(t0)		/* t3: POLY IDX */
	lw	t4,20(a0)		/* t4: primtop */
	sll	t3,t3,2			/* t3: t3*4 */
	beq	t2,zero,SNGT4_22	/* if (num==0) goto SNGT4_22 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

	lw	t6,4(a0)		/* t6: tagp */
	lw	t8,12(a0)		/* t8: offset */
	lw	t9,4(t6)		/* t9: otp->org */
	lw	t5,28(a0)		/* t5: vertop2 */

SNGT4_0:
	lw	a1,24(t7)		/* a1: Vertex0,uv2 */
	lw	a2,28(t7)		/* a2: Vertex1,uv3 */
	lw	t6,32(t7)		/* t6: Vertex2,Vertex3 */

	srl	v0,a1,16		/* v0: Vertex0 */
	sll	v0,v0,3			/* v0: Vertex0*8 */
	addu	t3,v0,t5		/* t3: vertop2[Vertex0] */
	lw	v0,4(t3)		/* v0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	v0,t0,SNGT4_21		/* if(otz==0xffffffff)goto SNGT4_21(clip) */
	nop	
	mtc2	v0,C2_SZ0		/* v0	-> C2_SZ0 */
	nop

	srl	v0,a2,16		/* v0: Vertex1 */
	sll	v0,v0,3			/* v0: Vertex1*8 */
	addu	t4,v0,t5		/* t4: vertop2[Vertex1] */
	lw	v0,4(t4)		/* v0: p,otz */
	nop	

	beq	v0,t0,SNGT4_21		/* if(otz==0xffffffff)goto SNGT4_21(clip) */
	nop	
	mtc2	v0,C2_SZ1		/* v0	-> C2_SZ1 */
	nop

	sll	v0,t6,16		/* v0: Vertex2 */
	srl	v0,v0,13		/* v0: Vertex2*8 */
	addu	v1,v0,t5		/* v1: vertop2[Vertex2] */
	lw	v0,4(v1)		/* v0: p,otz */
	nop	

	beq	v0,t0,SNGT4_21		/* if(otz==0xffffffff)goto SNGT4_21(clip) */
	nop	
	mtc2	v0,C2_SZ2		/* v0	-> C2_SZ2 */
	nop

	srl	v0,t6,16		/* v0: Vertex3 */
	sll	v0,v0,3			/* v0: Vertex3*8 */
	addu	a3,v0,t5		/* a3: vertop2[Vertex3] */
	lw	v0,4(a3)		/* v0: p,otz */
	nop	

	beq	v0,t0,SNGT4_21		/* if(otz==0xffffffff)goto SNGT4_21(clip) */
	nop	
	mtc2	v0,C2_SZ3		/* v0	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t3,0(v1)		/* t3: vx2,vy2 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t3,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	lw	t4,0(a3)		/* t4: vx3,vy3 */
	nop

#ifndef BACK_FACE
	/****/
	NCLIP				
	/****/
#endif /* !BACK_FACE */

	sw	v0,8(t1)		/* v0	-> p->x0,y0 */
	sw	t0,20(t1)		/* t0	-> p->x1,y1 */
	sw	t3,32(t1)		/* t3	-> p->x2,y2 */
	sw	t4,44(t1)		/* t4	-> p->x3,y3 */

#ifndef BACK_FACE
	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	bgtz	v0,SNGT4_10		/* if (opz > 0) goto SNGT4_10 */
	nop	

	mtc2	t4,C2_SXY0		/* vx3,vy3	-> C2_SXY0 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	t4,8(t1)		/* t4	-> p->x0,y0 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	bgtz	v0,SNGT4_21		/* if (opz > 0) goto SNGT4_21 */
	nop	

	/****/
	AVSZ3				
	/****/

	lw	v0,12(t7)		/* v0: r3,g3,b3,code */
	lw	v1,4(t7)		/* v1: r1,g1,b1 */
	lw	t4,8(t7)		/* t4: r2,g2,b2 */
	lw	t0,16(t7)		/* t0: uv0,cba */
	lw	t3,20(t7)		/* t3: uv1,tsb */
	and	v0,v0,0x00ffffff	/* v0: mask */
	or	v0,v0,0x36000000	/* v0: code=0x36 */
	and	t0,t0,0xffff0000	/* t0: cba */
	and	a2,a2,0x0000ffff	/* a2: uv3 */
	or	t0,t0,a2		/* t0: uv3,cba */
	sw	v0,4(t1)		/* v0	-> p->r0,g0,b0,cd */
	sw	v1,16(t1)		/* v1	-> p->r1,g1,b1 */
	sw	t4,28(t1)		/* t4	-> p->r2,g2,b2 */
	sw	t0,12(t1)		/* t0	-> p->uv0,cba */
	sw	t3,24(t1)		/* t3	-> p->uv1,tsb */
	sw	a1,36(t1)		/* a1	-> p->uv2 */

	mfc2	t4,C2_OTZ		/* t4: otz */
	lw	t6,8(a0)		/* a2: shift */

	subu	t4,t4,t8		/* sz = sz(otz)-otp->offset */
	srl	t4,t4,t6		/* sz>>shift */
	and	t4,t4,0x0ffff		/* mask ? */
	sll	t4,t4,2			/* sz*4 */
	addu	t4,t4,t9		/* &(tagp->org[sz]) */
	lw	v0,0(t4)		/* table[sz].p */
	li	v1,0x09000000		/* (sizeof(POLY_GT3)/4-1<<24) */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x09000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	j	SNGT4_21
	addu	t1,t1,40		/* out_packet++ */
#endif /* !BACK_FACE */

SNGT4_10:
	/****/
	AVSZ4				
	/****/

	lw	v0,0(t7)		/* v0: r0,g0,b0,cd */
	lw	v1,4(t7)		/* v1: r1,g1,b1 */
	lw	t4,8(t7)		/* t4: r2,g2,b2 */
	lw	t6,12(t7)		/* t6: r3,g3,b3 */
	lw	t0,16(t7)		/* t0: uv0,cba */
	lw	t3,20(t7)		/* t3: uv1,tsb */
	or	v0,v0,0x02000000	/* add ABE bit */
	sw	v0,4(t1)		/* v0	-> p->r0,g0,b0,cd */
	sw	v1,16(t1)		/* v1	-> p->r1,g1,b1 */
	sw	t4,28(t1)		/* t4	-> p->r2,g2,b2 */
	sw	t6,40(t1)		/* t6	-> p->r3,g3,b3 */
	sw	t0,12(t1)		/* t0	-> p->uv0,cba */
	sw	t3,24(t1)		/* t3	-> p->uv1,tsb */
	sw	a1,36(t1)		/* a1	-> p->uv2 */
	sw	a2,48(t1)		/* a2	-> p->uv3 */

	mfc2	t4,C2_OTZ		/* t4: otz */
	lw	t6,8(a0)		/* t6: shift */

	subu	t4,t4,t8		/* sz = sz(otz)-otp->offset */
	srl	t4,t4,t6		/* sz>>shift */
	and	t4,t4,0x0ffff		/* mask ? */
	sll	t4,t4,2			/* sz*4 */
	addu	t4,t4,t9		/* &(tagp->org[sz]) */
	lw	v0,0(t4)		/* table[sz].p */
	li	v1,0x0c000000		/* (sizeof(POLY_GT4)/4-1<<24) */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x0c000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	addu	t1,t1,52		/* out_packet++ */

SNGT4_21:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SNGT4_0		/* if(num!=0) goto SNGT4_0 */
	addu	t7,t7,36		/* primtop++ */

SNGT4_22:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	.set	reorder
	j	ra			/* return(primtop) */
