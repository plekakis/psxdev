/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_00200211		/* */

    	

	.text


/************************************************************************
* Tile Semi-Trans Flat/Texture Quad with lighting
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

GsU_00200211:

       	.set    noreorder

	lw	a3,0(a0)		/* a3: sp->primp */
	lw	t9,4(a0)		/* t9: otp; */
	lw	t8,0(a3)		/* *(sp->primp) */
	lw	t7,8(a0)		/* shift */
	addu	a3,a3,4			/* skip size, num */
	lw	a2,0(a3)		/* prim idx: *(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* n */
	beq	t8,0,TF4L_3		/* if(n==0) goto TF4L_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	li	v0,0x2e808080		/* v0: RGBcd */
	lw	t0,4(t9)		/* t0: table */
	mtc2    v0,C2_RGB 		/* R,G,B,cd <-- v0: GPU_COD_TF4 */

	lw	t1,12(a0)		/* t1: otp->offset */
	addu	v0,a0,16
	lw	t9,0(v0)		/* t9: pkt_base */

	lw	t2,20(a3)		/* t2: primtop->v0,v1 */
	lw	t3,24(a3)		/* t3: primtop->v2,v3 */

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

	sll	t4,t2,16		/* t4: v0 */
	srl	t4,t4,13		/* t4: v0*8 */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v0) */

TF4L_1:
	lwc2	C2_VXY0,(t4)		/* Vector 0 <--(vertop+primtop->v0) */
	lwc2	C2_VZ0,4(t4)

	srl	t5,t2,16		/* t5: v1 */
	sll	t5,t5,3			/* t5: v1*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v1) */
	lwc2	C2_VXY1,(t5)		/* Vector 1 <--(vertop+primtop->v1) */
	lwc2	C2_VZ1,4(t5)

	sll	t5,t3,16		/* t5: v2 */
	srl	t5,t5,13		/* t5: v2*8 */
	addu	t5,t5,a1		/* t5: (vertop+primtop->v2) */
	lwc2	C2_VXY2,(t5)		/* Vector 2 <--(vertop+primtop->v2) */
	lwc2	C2_VZ2,4(t5)
	nop
	nop

	/****/
        RTPT				
	/****/

	srl	v1,t3,16		/* v1: v3 */
	sll	v1,v1,3			/* v1: v3*8 */
	addu	v1,v1,a1		/* v1: (vertop+primtop->v3) */

	addu	a3,a3,28		/* primtop++ */

	lw	t2,20(a3)		/* t2: primtop->v0,v1 */
	lw	t3,24(a3)		/* t3: primtop->v2,v3 */

	sll	t4,t2,16		/* t4: v0 */
	srl	t4,t4,13		/* t4: v0*8 */
	addu	t4,t4,a1		/* t4: (vertop+primtop->v0) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,TF4L_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/

	lwc2	C2_VXY0,(v1)		/* Vector 0 <--(vertop+primtop->v3) */
	lwc2	C2_VZ0,4(v1)


	mfc2	v0,C2_MAC0		/* opz read */
	nop
	ble	v0,0,TF4L_2		/* Normal clip if MAC0(OPZ)<=0 */
	nop

        swc2    C2_SXY0,12(t9)		/* sxy0 -->12(pkt_base) */
        swc2    C2_SXY1,20(t9)		/* sxy1 -->20(pkt_base) */
	swc2    C2_SXY2,28(t9) 		/* sxy2 -->28(pkt_base) */
	nop
	nop

	/****/
        RTPS				
	/****/

	subu	v0,a3,28		/* v0: (primtop-28)=old primtop */
	lw	t6,16(v0)		/* primtop->uv3 */
	lw	v1,4(v0)		/* primtop->uv0 */
	srl	t5,t6,16		/* t5: n0<<16 */
	sll	t5,t5,3			/* t5: n0*8 */
	addu	t5,t5,a2		/* t5: (nortop+primtop->n0) */
	sw    	t6,40(t9) 		/* uv3 -->40(pkt_base) */
	sw    	v1,16(t9) 		/* uv0 -->16(pkt_base) */
	.set noat
	lw	AT,0(v0)		/* tw */
	lw	t6,12(v0)		/* primtop->uv2 */
	lw	v1,8(v0)		/* primtop->uv1 */
	sw	AT,4(t9)		/* tw -->4(pkt_base) */
	.set at
	sw    	t6,32(t9) 		/* uv2 -->32(pkt_base) */
	sw    	v1,24(t9) 		/* uv1 -->24(pkt_base) */

	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,TF4L_2		/* clip if flag <0 */
	nop


	/****/
	AVSZ4				
	/****/

	lwc2	C2_VXY0,(t5)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t5)


	mfc2	t6,C2_OTZ		/* t6: otz */

	nop
	nop

	/****/
        NCCS				
	/****/
	
	subu 	t6,t6,t1		/* sz = sz(otz) - otp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - otp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(table[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */

	li	v0,0xb000000		/* (sizeof(POLY_FT4)/4-1)<<24)+2 */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_FT4)/4-1)<<24)+2 */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	.set noat
	li	AT,0xe2000000		/* tw reset */
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sw	AT,44(t9)		/* tw reset */
	.set at

	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) of v0 */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

	swc2    C2_SXY2,36(t9) 		/* sxy3 -->36(pkt_base) */
	swc2	C2_RGB2,8(t9)		/* R2,G2,B2,cd -->8(pkt_base) */

	addu	t9,t9,48		/* pkt_base++; */

TF4L_2:

	subu	t8,t8,1
	bne	t8,0,TF4L_1		/* if(primtop != primtop+n) goto TF4L_1 */
	nop
TF4L_3:
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
