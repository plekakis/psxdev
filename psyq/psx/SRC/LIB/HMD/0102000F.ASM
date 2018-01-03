/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_0102000f

	
	.text
	

/************************************************************************
* Colored Shared Gouraud Texture Triangle with fog
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

GsU_0102000f:

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
	beq	t2,zero,SGT3_10		/* if (num==0) goto SGT3_10 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

SGT3_0:
	lw	a1,24(t7)		/* a1: Vertex0,Normal0 */
	lw	a2,28(t7)		/* a2: Vertex1,Normal1 */
	lw	a3,32(t7)		/* a3: Vertex2,Normal2 */

	srl	s0,a1,16		/* s0: Vertex0 */
	sll	s0,s0,3			/* s0: Vertex0*8 */
	addu	t3,s0,t5		/* t3: vertop2[Vertex0] */
	lw	s0,4(t3)		/* s0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	s0,t0,SGT3_9		/* if(otz==0xffffffff)goto SGT3_9(clip) */
	nop	
	mtc2	s0,C2_SZ1		/* s0	-> C2_SZ1 */
	nop

	srl	s1,a2,16		/* s1: Vertex1 */
	sll	s1,s1,3			/* s1: Vertex1*8 */
	addu	t4,s1,t5		/* t4: vertop2[Vertex1] */
	lw	s1,4(t4)		/* s1: p,otz */
	nop	

	beq	s1,t0,SGT3_9		/* if(otz==0xffffffff)goto SGT3_9(clip) */
	nop	
	mtc2	s1,C2_SZ2		/* s1	-> C2_SZ2 */
	nop

	srl	t8,a3,16		/* t8: Vertex2 */
	sll	t8,t8,3			/* t8: Vertex2*8 */
	addu	v1,t8,t5		/* v1: vertop2[Vertex2] */
	lw	t8,4(v1)		/* t8: p,otz */
	nop	

	beq	t8,t0,SGT3_9		/* if(otz==0xffffffff)goto SGT3_9(clip) */
	nop	
	mtc2	t8,C2_SZ3		/* t8	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t3,0(v1)		/* t3: vx2,vy2 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t3,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	v0,8(t1)		/* v0	-> p->x0,y0 */
	sw	t0,20(t1)		/* t0	-> p->x1,y1 */
	sw	t3,32(t1)		/* t3	-> p->x2,y2 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	blez	v0,SGT3_9		/* if (opz <= 0) goto SGT3_9 */
	nop	

	sll	v0,a1,16		/* v0: Normal0 */
	lw	a1,0(t7)		/* a1: r0,g0,b0,code */
	srl	t9,s0,16		/* t9: p */
	j	SGT3_3
	li	t4,0			/* t4: 0 */
SGT3_1:
	sll	v0,a2,16		/* v0: Normal1 */
	lw	a1,4(t7)		/* a1: r1,g1,b1,code */
	srl	t9,s1,16		/* t9: p */
	j	SGT3_3
	li	t4,1			/* t4: 1 */
SGT3_2:
	sll	v0,a3,16		/* v0: Normal2 */
	lw	a1,8(t7)		/* a1: r2,g2,b2,code */
	srl	t9,t8,16		/* t9: p */
	li	t4,2			/* t4: 2 */
SGT3_3:

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

	mtc2	t9,C2_IR0		/* dp */
	nop
	li	t9,0x34000000		/* t9: 0x34 (POLY_GT3) */

	mfc2	t3,C2_IR1		/* t3: rgb.vx */
	mfc2	v0,C2_IR2		/* v0: rgb.vy */
	mfc2	v1,C2_IR3		/* v1: rgb.vz */
	nop

	blt	t3,255,SGT3_4		/* if (rgb.vx<255) goto SGT3_4 */
	nop
	li	t3,255			/* t3: 255 */
SGT3_4:
	blt	v0,255,SGT3_5		/* if (rgb.vy<255) goto SGT3_5 */
	nop
	li	v0,255			/* v0: 255 */
SGT3_5:
	blt	v1,255,SGT3_6		/* if (rgb.vz<255) goto SGT3_6 */
	nop
	li	v1,255			/* v1: 255 */

SGT3_6:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	v1,v1,16		/* rgb.vz<<16 */
	or	t0,t3,v0		/* rgb.vy|rgb.vx */
	or	t0,t0,v1		/* rgb.vz|rgb.vy|rgb.vx */
	or	t0,t0,t9		/* 0x34|rgb.vz|rgb.vy|rgb.vx */
	mtc2	t0,C2_RGB		/* rgbcd->C2_RGB */
	nop
	nop

	/****/
	DPCS				
	/****/

	beq	t4,2,SGT3_8		/* if (t4==2) goto SGT3_8 */
	nop
	beq	t4,1,SGT3_7		/* if (t4==1) goto SGT3_7 */
	nop

	swc2	C2_RGB2,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	nop
	j	SGT3_1			/* goto SGT3_1 */
	nop

SGT3_7:
	swc2	C2_RGB2,16(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	nop
	j	SGT3_2			/* goto SGT3_2 */
	nop

SGT3_8:
	swc2	C2_RGB2,28(t1)		/* rgbcd	-> p->r2,g2,b2,code */
	nop
	nop

	/****/
	AVSZ3				
	/****/

	lw	a1,12(t7)		/* a1: uv0 */
	lw	a2,16(t7)		/* a2: uv1 */
	lw	a3,20(t7)		/* a3: uv2 */
	sw	a1,12(t1)		/* uv0	-> p->u0,v0 */
	sw	a2,24(t1)		/* uv1	-> p->u1,v1 */
	sw	a3,36(t1)		/* uv2	-> p->u2,v2 */
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

	addu	t1,t1,40		/* out_packet++ */

SGT3_9:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SGT3_0		/* if(num!=0) goto SGT3_0 */
	addu	t7,t7,36		/* primtop++ */

SGT3_10:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	lw	s0,24(a0)		/* restore s0 register */
	lw	s1,32(a0)		/* restore s1 register */

	.set	reorder
	j	ra			/* return(primtop) */
