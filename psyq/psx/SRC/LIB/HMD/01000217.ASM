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
	.globl	GsU_01200217
#else /* SEMI_TRANS */
	.globl	GsU_01000217
#endif /* SEMI_TRANS */

	
	.text
	

/************************************************************************
* (Semi-Trans) Tile Colored Shared Gouraud Texture Quad
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

#ifdef SEMI_TRANS
GsU_01200217:
#else /* SEMI_TRANS */
GsU_01000217:
#endif /* SEMI_TRANS */

	.set	noreorder

	sw	s0,24(a0)		/* save s0 register */
	sw	s1,32(a0)		/* save s1 register */

	lw	t0,0(a0)		/* t0: primp */
	lw	t1,16(a0)		/* t1: out_packetp */
	lhu	t2,2(t0)		/* t2: num */
	lw	t3,4(t0)		/* t3: POLY IDX */
	lw	t4,20(a0)		/* t4: primtop */
	lw	t5,28(a0)		/* t5: vertop2 */
	lw	t6,36(a0)		/* t6: nortop2 */
	sll	t3,t3,2			/* t3: t3*4 */
	beq	t2,zero,SGT4_27		/* if (num==0) goto SGT4_27 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

	sw	s2,16(a0)		/* save s2 register */

SGT4_0:
	lw	a1,32(t7)		/* a1: Vertex0,uv3 */
	lw	a2,36(t7)		/* a2: Vertex1,Normal1 */
	lw	a3,40(t7)		/* a3: Vertex2,Normal2 */
	lw	t9,44(t7)		/* t9: Vertex3,Normal3 */

	srl	v0,a1,16		/* v0: Vertex0 */
	sll	v0,v0,3			/* v0: Vertex0*8 */
	addu	t3,v0,t5		/* t3: vertop2[Vertex0] */
	lw	v0,4(t3)		/* v0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	v0,t0,SGT4_26		/* if(otz==0xffffffff)goto SGT4_26(clip) */
	nop	
	mtc2	v0,C2_SZ0		/* v0	-> C2_SZ0 */
	nop

	srl	v0,a2,16		/* v0: Vertex1 */
	sll	v0,v0,3			/* v0: Vertex1*8 */
	addu	t4,v0,t5		/* t4: vertop2[Vertex1] */
	lw	v0,4(t4)		/* v0: p,otz */
	nop	

	beq	v0,t0,SGT4_26		/* if(otz==0xffffffff)goto SGT4_26(clip) */
	nop	
	mtc2	v0,C2_SZ1		/* v0	-> C2_SZ1 */
	nop

	srl	v0,a3,16		/* v0: Vertex2 */
	sll	v0,v0,3			/* v0: Vertex2*8 */
	addu	v1,v0,t5		/* v1: vertop2[Vertex2] */
	lw	v0,4(v1)		/* v0: p,otz */
	nop	

	beq	v0,t0,SGT4_26		/* if(otz==0xffffffff)goto SGT4_26(clip) */
	nop	
	mtc2	v0,C2_SZ2		/* v0	-> C2_SZ2 */
	nop

	srl	v0,t9,16		/* v0: Vertex3 */
	sll	v0,v0,3			/* v0: Vertex3*8 */
	addu	s0,v0,t5		/* s0: vertop2[Vertex3] */
	lw	v0,4(s0)		/* v0: p,otz */
	nop	

	beq	v0,t0,SGT4_26		/* if(otz==0xffffffff)goto SGT4_26(clip) */
	nop	
	mtc2	v0,C2_SZ3		/* v0	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t3,0(v1)		/* t3: vx2,vy2 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t3,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	lw	t4,0(s0)		/* t4: vx3,vy3 */
	lw	s2,28(t7)		/* s2: Normal0,uv2 */

	/****/
	NCLIP				
	/****/

	sw	v0,12(t1)		/* v0	-> p->x0,y0 */
	sw	t0,24(t1)		/* t0	-> p->x1,y1 */
	sw	t3,36(t1)		/* t3	-> p->x2,y2 */
	sw	t4,48(t1)		/* t4	-> p->x3,y3 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
#ifdef SEMI_TRANS
	li	s1,0x3e000000		/* s1: 0x3e (POLY_GT4|ABE) */
#else /* SEMI_TRANS */
	li	s1,0x3c000000		/* s1: 0x3c (POLY_GT4) */
#endif /* SEMI_TRANS */
	bgtz	v0,SGT4_12		/* if (opz > 0) goto SGT4_12 */
	nop	

	mtc2	t4,C2_SXY0		/* vx3,vy3	-> C2_SXY0 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	t4,12(t1)		/* t4	-> p->x0,y0 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
#ifdef SEMI_TRANS
	li	s1,0x36000000		/* s1: 0x36 (POLY_GT3|ABE) */
#else /* SEMI_TRANS */
	li	s1,0x34000000		/* s1: 0x34 (POLY_GT3) */
#endif /* SEMI_TRANS */
	bgtz	v0,SGT4_26		/* if (opz > 0) goto SGT4_26 */
	nop	

	lw	a1,16(t7)		/* a1: r0,g0,b0,code */
	sll	v0,t9,16		/* v0: Normal3 */
	j	SGT4_3
	li	t4,0			/* t4: 0 */
SGT4_1:
	lw	a1,8(t7)		/* a1: r1,g1,b1,code */
	sll	v0,a2,16		/* v0: Normal1 */
	j	SGT4_3
	li	t4,1			/* t4: 1 */
SGT4_2:
	lw	a1,12(t7)		/* a1: r2,g2,b2,code */
	sll	v0,a3,16		/* v0: Normal2 */
	li	t4,2			/* t4: 2 */
SGT4_3:
	sll	v1,a1,24		/* v1: a1<<24 */
	srl	v1,v1,24		/* v1: r0 */
	sll	t0,a1,16		/* t0: a1<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	t3,a1,8			/* t3: a1<<8 */
	srl	t3,t3,24		/* t3: b0 */

	ctc2	v1,C2_R11R12		/* r0,0	-> C2_R11R12 */
	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	t3,C2_R33		/* b0	-> C2_R33 */

	srl	v0,v0,13		/* v0: Normal0*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normal0] */
	lw	v0,0(t0)		/* v0: LTr,LTg */
	lw	v1,4(t0)		/* v1: LTb */
	sll	t3,v0,16		/* t3: v0<<16 */
	srl	t3,t3,16		/* t3: LTr */
	srl	v0,v0,16		/* v0: LTg */
	mtc2	t3,C2_IR1
	mtc2	v0,C2_IR2
	mtc2	v1,C2_IR3
	nop	
	nop	
	
	/****/
	MVMVA(1,0,3,3,0)		/* R-Matrix*ir */
	/****/

	mfc2	t3,C2_IR1		/* t3: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	v1,C2_IR3		/* v1: rgb.vz */
	nop

	blt	t3,255,SGT4_5		/* if (rgb.vx<255) goto SGT4_5 */
	nop
	li	t3,255			/* t3: 255 */
SGT4_5:
	blt	v0,255,SGT4_7		/* if (rgb.vy<255) goto SGT4_7 */
	nop
	li	v0,255			/* v0: 255 */
SGT4_7:
	blt	v1,255,SGT4_9		/* if (rgb.vz<255) goto SGT4_9 */
	nop
	li	v1,255			/* v1: 255 */
SGT4_9:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	v1,v1,16		/* rgb.vz<<16 */
	or	t0,t3,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,v1		/* rgb.vz|rgb.vy|rgb.vx */
	beq	t4,2,SGT4_11		/* if (t4==2) goto SGT4_11 */
	nop
	beq	t4,1,SGT4_10		/* if (t4==1) goto SGT4_10 */
	nop

	or	t0,t0,s1		/* 0x34(0x36)|rgb.vz|rgb.vy|rgb.vx */
	j	SGT4_1			/* goto SGT4_1 */
	sw	t0,8(t1)		/* rgbcd	-> p->r0,g0,b0,code */

SGT4_10:
	j	SGT4_2			/* goto SGT4_2 */
	sw	t0,20(t1)		/* rgbcd	-> p->r1,g1,b1 */

SGT4_11:
	sw	t0,32(t1)		/* rgbcd	-> p->r2,g2,b2 */

	/****/
	AVSZ3				
	/****/

	lw	t0,20(t7)		/* t0: uv0,cba */
	lw	a2,24(t7)		/* a2: uv1 */
	lw	a1,28(t7)		/* a1: uv2 */
	lw	s2,32(t7)		/* s2: uv3 */
	lw	v0,0(t7)		/* tw */
	and	t0,t0,0xffff0000	/* t0: cba */
	and	a1,a1,0x0000ffff	/* a1: uv3 */
	or	a1,a1,t0		/* a1: uv3,cba */
	sw	a1,16(t1)		/* uv3	-> p->u0,v0 */
	sw	a2,28(t1)		/* uv1	-> p->u1,v1 */
	sw	s2,40(t1)		/* uv2	-> p->u2,v2 */
	sw	v0,4(t1)		/* tw */
	lw	t0,4(a0)		/* t0: tagp */
	lw	a1,12(a0)		/* a1: offset */
	lw	a2,8(a0)		/* a2: shift */
	lw	a3,4(t0)		/* a3: otp->org */

	mfc2	t4,C2_OTZ		/* t4: otz */
	nop

	subu	t4,t4,a1		/* sz = sz(otz)-otp->offset */
	srl	t4,t4,a2		/* sz>>shift */
	and	t4,t4,0x0ffff		/* mask ? */
	sll	t4,t4,2			/* sz*4 */
	addu	t4,t4,a3		/* &(tagp->org[sz]) */
	lw	v0,0(t4)		/* table[sz].p */
	li	v1,0x0b000000		/* (sizeof(POLY_GT3)/4-1<<24)+2 */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x09000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */

	.set noat
	li	AT,0xe2000000		/* tw reset */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */
	sw	AT,44(t1)		/* tw reset */
	.set at

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	j	SGT4_26
	addu	t1,t1,48		/* out_packet++ */

SGT4_12:
	srl	v0,s2,16		/* v0: Normal0 */
	sll	v0,v0,16		
	lw	t3,4(t7)		/* t3: r0,g0,b0,code */
	nop
	j	SGT4_16
	li	t4,0			/* t4: 0 */
SGT4_13:
	sll	v0,a2,16		/* v0: Normal1 */
	lw	t3,8(t7)		/* t3: r1,g1,b1,code */
	nop
	j	SGT4_16
	li	t4,1			/* t4: 1 */
SGT4_14:
	sll	v0,a3,16		/* v0: Normal2 */
	lw	t3,12(t7)		/* t3: r2,g2,b2,code */
	nop
	j	SGT4_16
	li	t4,2			/* t4: 2 */
SGT4_15:
	sll	v0,t9,16		/* v0: Normal3 */
	lw	t3,16(t7)		/* t3: r3,g3,b3,code */
	nop
	li	t4,3			/* t4: 3 */
SGT4_16:
	sll	v1,t3,24		/* v1: t3<<24 */
	srl	v1,v1,24		/* v1: r0 */
	sll	t0,t3,16		/* t0: t3<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	t3,t3,8			/* t3: t3<<8 */
	srl	t3,t3,24		/* t3: b0 */

	ctc2	v1,C2_R11R12		/* r0,0	-> C2_R11R12 */
	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	t3,C2_R33		/* b0	-> C2_R33 */

	srl	v0,v0,13		/* v0: Normal0*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normal0] */
	lw	v0,0(t0)		/* v0: LTr,LTg */
	lw	v1,4(t0)		/* v1: LTb */
	sll	t3,v0,16		/* t3: v0<<16 */
	srl	t3,t3,16		/* t3: LTr */
	srl	v0,v0,16		/* v0: LTg */
	mtc2	t3,C2_IR1
	mtc2	v0,C2_IR2
	mtc2	v1,C2_IR3
	nop	
	nop	
	
	/****/
	MVMVA(1,0,3,3,0)		/* R-Matrix*ir */
	/****/

	mfc2	t3,C2_IR1		/* t3: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	v1,C2_IR3		/* v1: rgb.vz */
	nop

	blt	t3,255,SGT4_18		/* if (rgb.vx<255) goto SGT4_18 */
	nop
	li	t3,255			/* t3: 255 */
SGT4_18:
	blt	v0,255,SGT4_20		/* if (rgb.vy<255) goto SGT4_20 */
	nop
	li	v0,255			/* v0: 255 */
SGT4_20:
	blt	v1,255,SGT4_22		/* if (rgb.vz<255) goto SGT4_22 */
	nop
	li	v1,255			/* v1: 255 */
SGT4_22:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	v1,v1,16		/* rgb.vz<<16 */
	or	t0,t3,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,v1		/* rgb.vz|rgb.vy|rgb.vx */
	beq	t4,3,SGT4_25		/* if (t4==3) goto SGT4_25 */
	nop
	beq	t4,2,SGT4_24		/* if (t4==2) goto SGT4_24 */
	nop
	beq	t4,1,SGT4_23		/* if (t4==1) goto SGT4_23 */
	nop

	or	t0,t0,s1		/* 0x3c(0x3e)|rgb.vz|rgb.vy|rgb.vx */
	j	SGT4_13			/* goto SGT4_13 */
	sw	t0,8(t1)		/* rgbcd	-> p->r0,g0,b0,code */

SGT4_23:
	j	SGT4_14			/* goto SGT4_14 */
	sw	t0,20(t1)		/* rgbcd	-> p->r1,g1,b1 */

SGT4_24:
	j	SGT4_15			/* goto SGT4_15 */
	sw	t0,32(t1)		/* rgbcd	-> p->r2,g2,b2 */

SGT4_25:
	sw	t0,44(t1)		/* rgbcd	-> p->r3,g3,b3 */

	/****/
	AVSZ4				
	/****/

	lw	a2,20(t7)		/* a2: uv0 */
	lw	a3,24(t7)		/* a3: uv1 */
	lw	s2,28(t7)		/* s2: uv2 */
	lw	a1,32(t7)		/* a1: uv3 */
	sw	a2,16(t1)		/* uv0	-> p->u0,v0 */
	sw	a3,28(t1)		/* uv1	-> p->u1,v1 */
	sw	s2,40(t1)		/* uv2	-> p->u2,v2 */
	sw	a1,52(t1)		/* uv3	-> p->u3,v3 */
	lw	v0,0(t7)		/* tw */
	lw	t0,4(a0)		/* t0: tagp */
	lw	a1,12(a0)		/* a1: offset */
	lw	a2,8(a0)		/* a2: shift */
	lw	a3,4(t0)		/* a3: otp->org */
	sw	v0,4(t1)		/* tw */

	mfc2	t4,C2_OTZ		/* t4: otz */
	nop

	subu	t4,t4,a1		/* sz = sz(otz)-otp->offset */
	srl	t4,t4,a2		/* sz>>shift */
	and	t4,t4,0x0ffff		/* mask ? */
	sll	t4,t4,2			/* sz*4 */
	addu	t4,t4,a3		/* &(tagp->org[sz]) */
	lw	v0,0(t4)		/* table[sz].p */
	li	v1,0x0e000000		/* (sizeof(POLY_GT4)/4-1<<24)+2 */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x0c000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */

	.set noat
	li	AT,0xe2000000		/* tw reset */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */
	sw	AT,56(t1)		/* tw reset */
	.set at

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	addu	t1,t1,60		/* out_packet++ */

SGT4_26:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SGT4_0		/* if(num!=0) goto SGT4_0 */
	addu	t7,t7,48		/* primtop++ */

SGT4_27:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	lw	s0,24(a0)		/* restore s0 register */
	lw	s1,32(a0)		/* restore s1 register */
	lw	s2,16(a0)		/* restore s2 register */

	.set	reorder
	j	ra			/* return(primtop) */
