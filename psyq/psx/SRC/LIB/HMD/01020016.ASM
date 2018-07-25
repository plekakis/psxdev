/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_01020016

	
	.text
	

/************************************************************************
* Colored Shared Gouraud Quad with fog
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

GsU_01020016:

	.set	noreorder

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

	sw	s0,16(a0)		/* save s0 register */
	sw	s1,24(a0)		/* save s1 register */
	sw	s2,32(a0)		/* save s2 register */

SG4_0:
	lw	a1,16(t7)		/* a1: Vertex0,Normal0 */
	lw	a2,20(t7)		/* a2: Vertex1,Normal1 */
	lw	a3,24(t7)		/* a3: Vertex2,Normal2 */
	lw	t9,28(t7)		/* t9: Vertex3,Normal3 */

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
	addu	s0,v0,t5		/* v1: vertop2[Vertex3] */
	lw	v0,4(s0)		/* v0: p,otz */
	nop	

	beq	v0,t0,SG4_21		/* if (otz==0xffffffff) goto SG4_21(clip) */
	nop	
	mtc2	v0,C2_SZ3		/* v0	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t8,0(v1)		/* t8: vx2,vy2 */
	lw	s1,0(s0)		/* s1: vx3,vy3 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t8,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	v0,8(t1)		/* v0	-> p->x0,y0 */
	sw	t0,16(t1)		/* t0	-> p->x1,y1 */
	sw	t8,24(t1)		/* t8	-> p->x2,y2 */
	sw	s1,32(t1)		/* s1	-> p->x3,y3 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop

	bgtz	v0,SG4_10		/* if (opz > 0) goto SG4_10 */
	nop	

	mtc2	s1,C2_SXY0		/* vx3,vy3	-> C2_SXY0 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	s1,8(t1)		/* s1	-> p->x0,y0 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	bgtz	v0,SG4_21		/* if (opz > 0) goto SG4_21 */
	nop	

	lhu	s1,6(s0)		/* s1: p */
	sll	v0,t9,16		/* v0: Normal3 */
	lw	a1,12(t7)		/* a1: r0,g0,b0,code */
	li	t8,0			/* t8: 0 */

SG4_1:
	sll	s2,a1,24		/* s2: a1<<24 */
	srl	s2,s2,24		/* s2: r0 */
	ctc2	s2,C2_R11R12		/* r0,0	-> C2_R11R12 */
	sll	t0,a1,16		/* t0: a1<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	s2,a1,8			/* s2: a1<<8 */
	srl	s2,s2,24		/* s2: b0 */

	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	s2,C2_R33		/* b0	-> C2_R33 */

	srl	v0,v0,13		/* v0: Normalx*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normalx] */
	lw	v0,0(t0)		/* v0: LTr,LTg */
	lw	s2,4(t0)		/* s2: LTb */
	sll	a1,v0,16		/* a1: v0<<16 */
	srl	a1,a1,16		/* a1: LTr */
	srl	v0,v0,16		/* v0: LTg */
	mtc2	a1,C2_IR1
	mtc2	v0,C2_IR2
	mtc2	s2,C2_IR3
	nop	
	nop	
	
	/****/
	MVMVA(1,0,3,3,0)		/* R-Matrix*ir */
	/****/

	mtc2	s1,C2_IR0		/* dp */
	nop
	li	s1,0x30000000		/* s1: 0x30 (POLY_G3) */

	mfc2	a1,C2_IR1		/* a1: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	s2,C2_IR3		/* s2: rgb.vz */
	nop
	blt	a1,255,SG4_3		/* if (rgb.vx<255) goto SG4_3 */
	nop
	li	a1,255			/* a1: 255 */
SG4_3:
	blt	v0,255,SG4_5		/* if (rgb.vy<255) goto SG4_5 */
	nop
	li	v0,255			/* v0: 255 */
SG4_5:
	blt	s2,255,SG4_7		/* if (rgb.vz<255) goto SG4_7 */
	nop
	li	s2,255			/* s2: 255 */

SG4_7:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	s2,s2,16		/* rgb.vz<<16 */
	or	t0,a1,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,s2		/* rgb.vz|rgb.vy|rgb.vx */
	or	t0,t0,s1		/* 0x30|rgb.vz|rgb.vy|rgb.vx */
	mtc2	t0,C2_RGB		/* rgbcd -> C2_RGB */
	nop
	nop

	/****/
	DPCS				
	/****/

	beq	t8,2,SG4_9		/* if (t8==2) goto SG4_9 */
	nop
	beq	t8,1,SG4_8		/* if (t8==1) goto SG4_8 */
	nop

	swc2	C2_RGB2,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	sll	v0,a2,16		/* v0: Normal1 */
	lw	a1,4(t7)		/* a1: r1,g1,b1,code */
	lhu	s1,6(t4)		/* s1: p */
	nop
	j	SG4_1			/* goto SG4_1 */
	addu	t8,t8,1			/* t8++ */

SG4_8:
	swc2	C2_RGB2,12(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	sll	v0,a3,16		/* v0: Normal2 */
	lw	a1,8(t7)		/* a1: r2,g2,b2,code */
	lhu	s1,6(v1)		/* s1: p */
	nop
	j	SG4_1			/* goto SG4_1 */
	addu	t8,t8,1			/* t8++ */

SG4_9:
	swc2	C2_RGB2,20(t1)		/* rgbcd	-> p->r2,g2,b2,code */
	nop
	nop

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
	lhu	s1,6(t3)		/* s1: p */
	sll	v0,a1,16		/* v0: Normal0 */
	lw	a1,0(t7)		/* a1: r0,g0,b0,code */
	li	t8,0			/* t8: 0 */

SG4_11:
	sll	s2,a1,24		/* s2: a1<<24 */
	srl	s2,s2,24		/* s2: r0 */
	ctc2	s2,C2_R11R12		/* r0,0	-> C2_R11R12 */
	sll	t0,a1,16		/* t0: a1<<16 */
	srl	t0,t0,24		/* t0: g0 */
	sll	s2,a1,8			/* s2: a1<<8 */
	srl	s2,s2,24		/* s2: b0 */

	ctc2	zero,C2_R13R21		/* 0,0	-> C2_R13R21 */
	ctc2	t0,C2_R22R23		/* g0,0	-> C2_R22R23 */
	ctc2	zero,C2_R31R32		/* 0,0	-> C2_R31R32 */
	ctc2	s2,C2_R33		/* b0	-> C2_R33 */

	srl	v0,v0,13		/* v0: Normalx*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normalx] */
	lw	v0,0(t0)		/* v0: LTr,LTg */
	lw	s2,4(t0)		/* s2: LTb */
	sll	a1,v0,16		/* a1: v0<<16 */
	srl	a1,a1,16		/* a1: LTr */
	srl	v0,v0,16		/* v0: LTg */
	mtc2	a1,C2_IR1
	mtc2	v0,C2_IR2
	mtc2	s2,C2_IR3
	nop	
	nop	
	
	/****/
	MVMVA(1,0,3,3,0)		/* R-Matrix*ir */
	/****/

	mtc2	s1,C2_IR0		/* dp */
	nop
	li	s1,0x38000000		/* s1: 0x38 (POLY_G4) */

	mfc2	a1,C2_IR1		/* a1: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	s2,C2_IR3		/* s2: rgb.vz */
	nop
	blt	a1,255,SG4_13		/* if (rgb.vx<255) goto SG4_13 */
	nop
	li	a1,255			/* a1: 255 */
SG4_13:
	blt	v0,255,SG4_15		/* if (rgb.vy<255) goto SG4_15 */
	nop
	li	v0,255			/* v0: 255 */
SG4_15:
	blt	s2,255,SG4_17		/* if (rgb.vz<255) goto SG4_17 */
	nop
	li	s2,255			/* s2: 255 */
SG4_17:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	s2,s2,16		/* rgb.vz<<16 */
	or	t0,a1,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,s2		/* rgb.vz|rgb.vy|rgb.vx */
	or	t0,t0,s1		/* 0x38|rgb.vz|rgb.vy|rgb.vx */
	mtc2	t0,C2_RGB		/* rgbcd -> C2_RGB */
	nop
	nop

	/****/
	DPCS				
	/****/

	beq	t8,3,SG4_20		/* if (t8==3) goto SG4_20 */
	nop
	beq	t8,2,SG4_19		/* if (t8==2) goto SG4_19 */
	nop
	beq	t8,1,SG4_18		/* if (t8==1) goto SG4_18 */
	nop

	swc2	C2_RGB2,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	sll	v0,a2,16		/* v0: Normal1 */
	lw	a1,4(t7)		/* a1: r1,g1,b1,code */
	lhu	s1,6(t3)		/* s1: p */
	nop
	j	SG4_11			/* goto SG4_11 */
	addu	t8,t8,1			/* t8++ */

SG4_18:
	swc2	C2_RGB2,12(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	sll	v0,a3,16		/* v0: Normal2 */
	lw	a1,8(t7)		/* a1: r2,g2,b2,code */
	lhu	s1,6(t4)		/* s1: p */
	nop
	j	SG4_11			/* goto SG4_11 */
	addu	t8,t8,1			/* t8++ */

SG4_19:
	swc2	C2_RGB2,20(t1)		/* rgbcd	-> p->r2,g2,b2,code */
	sll	v0,t9,16		/* v0: Normal3 */
	lw	a1,12(t7)		/* a1: r3,g3,b3,code */
	lhu	s1,6(s0)		/* s1: p */
	nop
	j	SG4_11			/* goto SG4_11 */
	addu	t8,t8,1			/* t8++ */

SG4_20:
	swc2	C2_RGB2,28(t1)		/* rgbcd	-> p->r3,g3,b3,code */
	nop
	nop

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
	addu	t7,t7,32		/* primtop++ */

SG4_22:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	lw	s0,16(a0)		/* restore s0 register */
	lw	s1,24(a0)		/* restore s1 register */
	lw	s2,32(a0)		/* restore s2 register */

	.set	reorder
	j	ra			/* return(primtop) */
