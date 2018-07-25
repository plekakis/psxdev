/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_01200014

	
	.text
	

/************************************************************************
* Semi-Trans Shared Gouraud Quad (GsUSTPSG4)
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

GsU_01200014:

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
	beq	t2,zero,SG4_22		/* if (num==0) goto SG4_22 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

SG4_0:
	lw	a1,4(t7)		/* a1: Vertex0,Normal0 */
	lw	a2,8(t7)		/* a2: Vertex1,Normal1 */
	lw	a3,12(t7)		/* a3: Vertex2,Normal2 */
	lw	t9,16(t7)		/* t9: Vertex3,Normal3 */

	srl	v0,a1,16		/* v0: Vertex0 */
	sll	v0,v0,3			/* v0: Vertex0*8 */
	addu	t3,v0,t5		/* t3: vertop2[Vertex0] */
	lw	v0,4(t3)		/* v0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	v0,t0,SG4_21		/* if (otz==0xffffffff) goto SG4_21(clip) */
	nop	
	mtc2	v0,C2_SZ0		/* v0	-> C2_SZ0 */
	nop

	srl	v0,a2,16		/* v0: Vertex1 */
	sll	v0,v0,3			/* v0: Vertex1*8 */
	addu	t4,v0,t5		/* t4: vertop2[Vertex1] */
	lw	v0,4(t4)		/* v0: p,otz */
	nop	

	beq	v0,t0,SG4_21		/* if (otz==0xffffffff) goto SG4_21(clip) */
	nop	
	mtc2	v0,C2_SZ1		/* v0	-> C2_SZ1 */
	nop

	srl	v0,a3,16		/* v0: Vertex2 */
	sll	v0,v0,3			/* v0: Vertex2*8 */
	addu	v1,v0,t5		/* v1: vertop2[Vertex2] */
	lw	v0,4(v1)		/* v0: p,otz */
	nop	

	beq	v0,t0,SG4_21		/* if (otz==0xffffffff) goto SG4_21(clip) */
	nop	
	mtc2	v0,C2_SZ2		/* v0	-> C2_SZ2 */
	nop

	srl	v0,t9,16		/* v0: Vertex3 */
	sll	v0,v0,3			/* v0: Vertex3*8 */
	addu	s0,v0,t5		/* s0: vertop2[Vertex3] */
	lw	v0,4(s0)		/* v0: p,otz */
	nop	

	beq	v0,t0,SG4_21		/* if (otz==0xffffffff) goto SG4_21(clip) */
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
	nop

	/****/
	NCLIP				
	/****/

	sw	v0,8(t1)		/* v0	-> p->x0,y0 */
	sw	t0,16(t1)		/* t0	-> p->x1,y1 */
	sw	t3,24(t1)		/* t3	-> p->x2,y2 */
	sw	t4,32(t1)		/* t4	-> p->x3,y3 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	li	s1,0x3a000000		/* s1: 0x3a (POLY_G4|ABE) */
	bgtz	v0,SG4_10		/* if (opz > 0) goto SG4_10 */
	nop	

	mtc2	t4,C2_SXY0		/* vx3,vy3	-> C2_SXY0 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	t4,8(t1)		/* t4	-> p->x0,y0 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	li	s1,0x32000000		/* s1: 0x32 (POLY_G3|ABE) */
	bgtz	v0,SG4_21		/* if (opz > 0) goto SG4_21 */
	nop	

	lw	v0,0(t7)		/* v0: r0,g0,b0,code */
	nop
	sll	v1,v0,24		/* v1: v0<<24 */
	srl	v1,v1,24		/* v1: r0 */
	sll	t0,v0,16		/* t0: v0<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	t3,v0,8			/* t3: v0<<8 */
	srl	t3,t3,24		/* t3: b0 */

	ctc2	v1,C2_R11R12		/* r0,0	-> C2_R11R12 */
	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	t3,C2_R33		/* b0	-> C2_R33 */

	li	t4,0			/* t4: 0 */
	sll	v0,t9,16		/* v0: Normal3 */

SG4_1:
	srl	v0,v0,13		/* v0: Normalx*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normalx] */
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
	MVMVA(1,0,3,3,0)				/* R-Matrix*ir */
	/****/

	mfc2	t3,C2_IR1		/* t3: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	v1,C2_IR3		/* v1: rgb.vz */
	nop
	blt	t3,255,SG4_3		/* if (rgb.vx<255) goto SG4_3 */
	nop
	li	t3,255			/* t3: 255 */
SG4_3:
	blt	v0,255,SG4_5		/* if (rgb.vy<255) goto SG4_5 */
	nop
	li	v0,255			/* v0: 255 */
SG4_5:
	blt	v1,255,SG4_7		/* if (rgb.vz<255) goto SG4_7 */
	nop
	li	v1,255			/* v1: 255 */

SG4_7:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	v1,v1,16		/* rgb.vz<<16 */
	or	t0,t3,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,v1		/* rgb.vz|rgb.vy|rgb.vx */

	beq	t4,2,SG4_9		/* if (t4==2) goto SG4_9 */
	nop
	beq	t4,1,SG4_8		/* if (t4==1) goto SG4_8 */
	nop

	or	t0,t0,s1		/* 0x32|rgb.vz|rgb.vy|rgb.vx */
	sw	t0,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	sll	v0,a2,16		/* v0: Normal1 */
	j	SG4_1			/* goto SG4_1 */
	addu	t4,t4,1			/* t4++ */

SG4_8:
	sw	t0,12(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	sll	v0,a3,16		/* v0: Normal2 */
	j	SG4_1			/* goto SG4_1 */
	addu	t4,t4,1			/* t4++ */

SG4_9:
	sw	t0,20(t1)		/* rgbcd	-> p->r3,g3,b3,code */

	/****/
	AVSZ3				
	/****/

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
	li	v1,0x06000000		/* (sizeof(POLY_G4)/4-1<<24) */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x08000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	j	SG4_21
	addu	t1,t1,28		/* out_packet++ */

SG4_10:
	lw	v0,0(t7)		/* v0: r0,g0,b0,code */
	nop
	sll	v1,v0,24		/* v1: v0<<24 */
	srl	v1,v1,24		/* v1: r0 */
	sll	t0,v0,16		/* t0: v0<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	t3,v0,8			/* t3: v0<<8 */
	srl	t3,t3,24		/* t3: b0 */

	ctc2	v1,C2_R11R12		/* r0,0	-> C2_R11R12 */
	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	t3,C2_R33		/* b0	-> C2_R33 */

	li	t4,0			/* t4: 0 */
	sll	v0,a1,16		/* v0: Normal0 */

SG4_11:
	srl	v0,v0,13		/* v0: Normalx*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normalx] */
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
	MVMVA(1,0,3,3,0)				/* R-Matrix*ir */
	/****/

	mfc2	t3,C2_IR1		/* t3: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	v1,C2_IR3		/* v1: rgb.vz */
	nop
	blt	t3,255,SG4_13		/* if (rgb.vx<255) goto SG4_13 */
	nop
	li	t3,255			/* t3: 255 */
SG4_13:
	blt	v0,255,SG4_15		/* if (rgb.vy<255) goto SG4_15 */
	nop
	li	v0,255			/* v0: 255 */
SG4_15:
	blt	v1,255,SG4_17		/* if (rgb.vz<255) goto SG4_17 */
	nop
	li	v1,255			/* v1: 255 */
SG4_17:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	v1,v1,16		/* rgb.vz<<16 */
	or	t0,t3,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,v1		/* rgb.vz|rgb.vy|rgb.vx */

	beq	t4,3,SG4_20		/* if (t4==3) goto SG4_20 */
	nop
	beq	t4,2,SG4_19		/* if (t4==2) goto SG4_19 */
	nop
	beq	t4,1,SG4_18		/* if (t4==1) goto SG4_18 */
	nop

	or	t0,t0,s1		/* 0x3a|rgb.vz|rgb.vy|rgb.vx */
	sw	t0,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	sll	v0,a2,16		/* v0: Normal1 */
	j	SG4_11			/* goto SG4_11 */
	addu	t4,t4,1			/* t4++ */

SG4_18:
	sw	t0,12(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	sll	v0,a3,16		/* v0: Normal2 */
	j	SG4_11			/* goto SG4_11 */
	addu	t4,t4,1			/* t4++ */

SG4_19:
	sw	t0,20(t1)		/* rgbcd	-> p->r2,g2,b2,code */
	sll	v0,t9,16		/* v0: Normal3 */
	j	SG4_11			/* goto SG4_11 */
	addu	t4,t4,1			/* t4++ */

SG4_20:
	sw	t0,28(t1)		/* rgbcd	-> p->r3,g3,b3,code */

	/****/
	AVSZ4				
	/****/

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
	li	v1,0x08000000		/* (sizeof(POLY_G4)/4-1<<24) */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x08000000) */
	sw	v1,0(t1)		/* *(long*)pkt_base = v1 */
	xor	v1,t1,v0		/* pkt_base xor table[sz].p */

	sll	t3,v1,8			
	srl	v1,t3,8			/* clear (31..24) */
	xor	t3,v1,v0		/* xor table[sz].p */
	sw	t3,0(t4)		/* table[sz].p = pkt_base */

	addu	t1,t1,36		/* out_packet++ */

SG4_21:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SG4_0		/* if(num!=0) goto SG4_0 */
	addu	t7,t7,20		/* primtop++ */

SG4_22:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	lw	s0,24(a0)		/* restore s0 register */
	lw	s1,32(a0)		/* restore s1 register */

	.set	reorder
	j	ra			/* return(primtop) */
