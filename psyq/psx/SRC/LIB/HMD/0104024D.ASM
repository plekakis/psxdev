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
	.globl	GsU_0114024d
#else /* BACK_FACE */
	.globl	GsU_0104024d
#endif /* BACK_FACE */

	
	.text
	

/************************************************************************
* (Back-Facing) Tile Shared Non-light Gouraud Texture Triangle (GsUSNGT3)
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
GsU_0114024d:
#else /* BACK_FACE */
GsU_0104024d:
#endif /* BACK_FACE */

	.set	noreorder

	lw	t0,0(a0)		/* t0: primp */
	lw	t1,16(a0)		/* t1: out_packetp */
	lhu	t2,2(t0)		/* t2: num */
	lw	t3,4(t0)		/* t3: POLY IDX */
	lw	t4,20(a0)		/* t4: primtop */
	sll	t3,t3,2			/* t3: t3*4 */
	beq	t2,zero,SNGT3_10	/* if (num==0) goto SNGT3_10 */
	addu	t7,t4,t3		/* t7: &primtop[POLY_IDX] */

	lw	t9,4(a0)		/* t9: tagp */
	lw	t6,12(a0)		/* t6: offset */
	lw	a3,4(t9)		/* a3: otp->org */
	lw	t5,28(a0)		/* t5: vertop2 */
	lw	t9,8(a0)		/* t9: shift */

SNGT3_0:
	lw	a1,24(t7)		/* a1: Vertex0,uv2 */
	lw	a2,28(t7)		/* a2: Vertex1,Vertex2 */

	srl	v0,a1,16		/* v0: Vertex0 */
	sll	v0,v0,3			/* v0: Vertex0*8 */
	addu	t3,v0,t5		/* t3: vertop2[Vertex0] */
	lw	v0,4(t3)		/* v0: p,otz */
	li	t0,-1			/* t0: 0xffffffff */

	beq	v0,t0,SNGT3_9		/* if(otz==0xffffffff) goto SNGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ1		/* v0	-> C2_SZ1 */
	nop

	sll	v0,a2,16		/* v0: Vertex1 */
	srl	v0,v0,13		/* v0: Vertex1*8 */
	addu	t4,v0,t5		/* t4: vertop2[Vertex1] */
	lw	v0,4(t4)		/* v0: p,otz */
	nop	

	beq	v0,t0,SNGT3_9		/* if(otz==0xffffffff) goto SNGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ2		/* v0	-> C2_SZ2 */
	nop

	srl	v0,a2,16		/* v0: Vertex2 */
	sll	v0,v0,3			/* v0: Vertex2*8 */
	addu	v1,v0,t5		/* v1: vertop2[Vertex2] */
	lw	v0,4(v1)		/* v0: p,otz */
	nop	

	beq	v0,t0,SNGT3_9		/* if(otz==0xffffffff) goto SNGT3_9(clip) */
	nop	
	mtc2	v0,C2_SZ3		/* v0	-> C2_SZ3 */
	nop

	lw	v0,0(t3)		/* v0: vx0,vy0 */
	lw	t0,0(t4)		/* t0: vx1,vy1 */
	lw	t3,0(v1)		/* t3: vx2,vy2 */
	mtc2	v0,C2_SXY0		/* vx0,vy0	-> C2_SXY0 */
	mtc2	t0,C2_SXY1		/* vx1,vy1	-> C2_SXY1 */
	mtc2	t3,C2_SXY2		/* vx2,vy2	-> C2_SXY2 */
	nop
	nop

#ifndef BACK_FACE
	/****/
	NCLIP				
	/****/
#endif /* !BACK_FACE */

	sw	v0,12(t1)		/* v0	-> p->x0,y0 */
	sw	t0,24(t1)		/* t0	-> p->x1,y1 */
	sw	t3,36(t1)		/* t3	-> p->x2,y2 */

#ifndef BACK_FACE
	mfc2	v0,C2_MAC0		/* v0: C2_MAC0 */
	nop
	blez	v0,SNGT3_9		/* if (opz <= 0) goto SNGT3_9 */
	nop	
#endif /* !BACK_FACE */

	/****/
	AVSZ3				
	/****/

	lw	v0,4(t7)		/* v0: r0,g0,b0,code */
	lw	v1,8(t7)		/* v1: r1,g1,b1 */
	lw	t4,12(t7)		/* t4: r2,g2,b2 */
	sw	v0,8(t1)		/* v0	-> p->r0,g0,b0,code */
	sw	v1,20(t1)		/* v1	-> p->r1,g1,b1,code */
	sw	t4,32(t1)		/* t4	-> p->r2,g2,b2,code */
	lw	t0,16(t7)		/* t0: uv0,cba */
	lw	t3,20(t7)		/* t3: uv1,tsb */
	lw	v0,0(t7)		/* tw */
	sw	t0,16(t1)		/* t0	-> p->uv0,cba */
	sw	t3,28(t1)		/* t3	-> p->uv1,tsb */
	sw	a1,40(t1)		/* a1	-> p->uv2 */
	sw	v0,4(t1)		/* t1 */

	mfc2	t4,C2_OTZ		/* t4: otz */
	nop

	subu	t4,t4,t6		/* sz = sz(otz)-otp->offset */
	srl	t4,t4,t9		/* sz>>shift */
	and	t4,t4,0x0ffff		/* mask ? */
	sll	t4,t4,2			/* sz*4 */
	addu	t4,t4,a3		/* &(tagp->org[sz]) */
	lw	v0,0(t4)		/* table[sz].p */
	li	v1,0xb000000		/* (sizeof(POLY_GT3)/4-1<<24)+2 */

	sll	t3,v0,8			
	srl	t3,t3,8			/* clear(31..24) of table[sz].p */
	or	v1,v1,t3		/* (table[sz].p|0x9000000) */
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

	addu	t1,t1,48		/* out_packet++ */

SNGT3_9:
	subu	t2,t2,1			/* num-- */
	bne	t2,0,SNGT3_0		/* if(num!=0) goto SNGT3_0 */
	addu	t7,t7,32		/* primtop++ */

SNGT3_10:
	lw	v0,0(a0)		/* sp->primp */
	sw	t1,GsOUT_PACKET_P	
	addu	v0,v0,8			/* sp->primp+2 */

	.set	reorder
	j	ra			/* return(primtop) */
