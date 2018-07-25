/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_0102000d

	
	.text
	

/************************************************************************
* Shared Fog Gouraud Texture Triangle (GsUSFGT3)
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

GsU_0102000d:

	.set	noreorder

	lw	t0,0(a0)		/* t0: primp */
	lw	t1,16(a0)		/* t1: out_packetp */
	lhu	t2,2(t0)		/* t2: num */
	lw	t3,4(t0)		/* t3: POLY IDX */
	lw	t4,20(a0)		/* t4: primtop */
	lw	t5,28(a0)		/* t5: vertop2 */
	lw	t6,36(a0)		/* t6: nortop2 */
	sll	t3,t3,2			/* t3: t3*4 */
	beq	t2,zero,SFGT3_10	/* if (num==0) goto SFGT3_10 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

SFGT3_0:
	lw	a1,12(t7)		/* a1: Vertex0,Normal0 */
	lw	a2,16(t7)		/* a2: Vertex1,Normal1 */
	lw	a3,20(t7)		/* a3: Vertex2,Normal2 */

	srl	v0,a1,16		/* v0: Vertex0 */
	sll	v0,v0,3			/* v0: Vertex0*8 */
	addu	t3,v0,t5		/* t3: vertop2[Vertex0] */
	lw	v0,4(t3)		/* v0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	v0,t0,SFGT3_9		/* if(otz==0xffffffff)goto SFGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ1		/* v0	-> C2_SZ1 */
	nop

	srl	v0,a2,16		/* v0: Vertex1 */
	sll	v0,v0,3			/* v0: Vertex1*8 */
	addu	t4,v0,t5		/* t4: vertop2[Vertex1] */
	lw	v0,4(t4)		/* v0: p,otz */
	nop	

	beq	v0,t0,SFGT3_9		/* if(otz==0xffffffff)goto SFGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ2		/* v0	-> C2_SZ2 */
	nop

	srl	v0,a3,16		/* v0: Vertex2 */
	sll	v0,v0,3			/* v0: Vertex2*8 */
	addu	v1,v0,t5		/* v1: vertop2[Vertex2] */
	lw	v0,4(v1)		/* v0: p,otz */
	nop	

	beq	v0,t0,SFGT3_9		/* if(otz==0xffffffff)goto SFGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ3		/* v0	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t8,0(v1)		/* t8: vx2,vy2 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t8,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	nop
	nop

	/****/
	NCLIP				
	/****/

	sw	v0,8(t1)		/* v0	-> p->x0,y0 */
	sw	t0,20(t1)		/* t0	-> p->x1,y1 */
	sw	t8,32(t1)		/* t8	-> p->x2,y2 */

	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	blez	v0,SFGT3_9		/* if (opz <= 0) goto SFGT3_9 */
	nop	

	sll	v0,a1,16		/* v0: Normal0 */
	move	t9,t3
	j	SFGT3_3
	li	t8,0			/* t8: 0 */
SFGT3_1:
	sll	v0,a2,16		/* v0: Normal1 */
	move	t9,t4
	j	SFGT3_3
	li	t8,1			/* t8: 1 */
SFGT3_2:
	sll	v0,a3,16		/* v0: Normal2 */
	move	t9,v1
	li	t8,2			/* t8: 2 */
SFGT3_3:
	lhu	t3,6(t9)		/* t9: p */

	srl	v0,v0,13		/* v0: Normal0*8 */
	addu	t0,v0,t6		/* t0: nortop2[Normal0] */

	mtc2	t3,C2_IR0		/* dp */

	lw	v0,0(t0)		/* v0: LTr,LTg */
	lw	t9,4(t0)		/* t9: LTb */
	sll	t3,v0,16		/* t3: v0<<16 */
	srl	t3,t3,21		/* t3: LTr>>5 */
	srl	v0,v0,21		/* v0: LTg>>5 */
	srl	t0,t9,5			/* t0: LTb>>5 */

	blt	t3,255,SFGT3_4		/* if (rgb.vx<255) goto SFGT3_4 */
	li	t9,0x34000000		/* t9: 0x34 (POLY_GT3) */
	li	t3,255			/* t3: 255 */
SFGT3_4:
	blt	v0,255,SFGT3_5		/* if (rgb.vy<255) goto SFGT3_5 */
	nop
	li	v0,255			/* v0: 255 */
SFGT3_5:
	blt	t0,255,SFGT3_6		/* if (rgb.vz<255) goto SFGT3_6 */
	nop
	li	t0,255			/* t0: 255 */

SFGT3_6:
	sll	v0,v0,8			/* rgb.vy<<8 */
	sll	t0,t0,16		/* rgb.vz<<16 */
	or	t3,t3,v0		/* rgb.vy|rgb.vx */
	or	t3,t3,t0		/* rgb.vz|rgb.vy|rgb.vx */
	or	t0,t3,t9		/* 0x34|rgb.vz|rgb.vy|rgb.vx */
	mtc2	t0,C2_RGB		/* rgbcd	-> C2_RGB */
	nop
	nop

	/****/
	DPCS				
	/****/

	beq	t8,2,SFGT3_8		/* if (t8==2) goto SFGT3_8 */
	nop
	beq	t8,1,SFGT3_7		/* if (t8==1) goto SFGT3_7 */
	nop

	swc2	C2_RGB2,4(t1)		/* rgbcd	-> p->r0,g0,b0,code */
	j	SFGT3_1			/* goto SFGT3_1 */
	nop

SFGT3_7:
	swc2	C2_RGB2,16(t1)		/* rgbcd	-> p->r1,g1,b1,code */
	j	SFGT3_2			/* goto SFGT3_2 */
	nop

SFGT3_8:
	swc2	C2_RGB2,28(t1)		/* rgbcd	-> p->r2,g2,b2,code */
	nop
	nop

	/****/
	AVSZ3				
	/****/

	lw	a1,0(t7)		/* a1: uv0 */
	lw	a2,4(t7)		/* a2: uv1 */
	lw	a3,8(t7)		/* a3: uv2 */
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

SFGT3_9:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SFGT3_0		/* if(num!=0) goto SFGT3_0 */
	addu	t7,t7,24		/* primtop++ */

SFGT3_10:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	.set	reorder
	j	ra			/* return(primtop) */
