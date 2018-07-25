/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_0002020d		/* */

	

	.text


/************************************************************************
* Tile Gouraud/Texture Triangle with fog
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

GsU_0002020d:

	.set    noreorder

	lw	a3,0(a0)		/* a3: sp->primp */
	lw	t9,4(a0)		/* t9: otp; */
	lw	t8,0(a3)		/* *(primp) */
	lw	t7,8(a0)		/* shift */
	addu	a3,a3,4			/* skip size, num */
	lw	a2,0(a3)		/* prim idx: *(sp->primp) */
	lw	a1,20(a0)		/* sp->primtop */
	sll	a2,a2,2			/* idx*4 */
	srl	t8,t8,16		/* n */
	beq	t8,0,TG3L_3		/* if(n==0) goto TG3L_3: */
	addu	a3,a1,a2		/* primtop: sp->primtop+idx */

	li	v0,0x34808080		/* v0: RGBcd */
	lw	t0,4(t9)		/* t0: table */
	mtc2    v0,C2_RGB 		/* R,G,B,cd <-- v0: GPU_COD_TF4 */

	lw	t1,12(a0)		/* t1: otp->offset */

	addu	v0,a0,16		/* t9: pkt_base */
	lw	t9,0(v0)

	lw	a1,24(a0)		/* a1: vertop */
	lw	a2,28(a0)		/* a2: nortop */

TG3L_1:

	lw	t2,16(a3)		/* t2: primtop->v0,n0 */
	lw	t3,20(a3)		/* t3: primtop->v1,n1 */
	lw	t4,24(a3)		/* t4: primtop->v2,n2 */
TG3L_1L:
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
	nop

	nop
	/****/
        RTPT				
	/****/

	sll	t2,t2,16		/* t2: (n0 & 0x0000ffff) * 8 */
	srl	t2,t2,13
	addu	t2,t2,a2		/* t2: (nortop+primtop->n0) */
	sll	t3,t3,16		/* t3: (n1 & 0x0000ffff) * 8 */
	srl	t3,t3,13
	addu	t3,t3,a2		/* t3: (nortop+primtop->n1) */
	sll	t4,t4,16		/* t4: (n2 & 0x0000ffff) * 8 */
	srl	t4,t4,13
	addu	t4,t4,a2		/* t4: (nortop+primtop->n2) */


	cfc2	v0,C2_FLAG		/* v0: flag */
	nop
	blt	v0,0,TG3L_2		/* clip if flag <0 */
	nop

	/****/
        NCLIP				
	/****/


	mfc2	v0,C2_MAC0		/* opz read */
	nop
#ifdef NORMAL_FLIP
	bge	v0,0,TG3L_2		/* Normal clip if MAC0(OPZ)>=0 (FLIP) */
#else
	ble	v0,0,TG3L_2		/* Normal clip if MAC0(OPZ)<=0 */
#endif
	nop


	/****/
	AVSZ3				
	/****/

	lwc2	C2_VXY0,(t2)		/* NX0,NY0,NZ0 <--(nortop+primtop->n0) */
	lwc2	C2_VZ0,4(t2)
        lwc2    C2_VXY1,(t3)            /* NX1,NY1,NZ1 <--(nortop+primtop->n1) */
        lwc2    C2_VZ1,4(t3)
        lwc2    C2_VXY2,(t4)            /* NX2,NY2,NZ2 <--(nortop+primtop->n2) */
        lwc2    C2_VZ2,4(t4)


	mfc2	t6,C2_OTZ		/* t6: otz */

	nop

	nop
	/****/
        NCDT				
	/****/
	
	subu 	t6,t6,t1		/* sz = sz(otz) - otp->offset */
	srl 	t6,t6,t7		/* sz = (sz(otz) - otp->offset)>>shift; */
	and	t6,t6,0x0ffff		/* mask ? */
	sll	t6,t6,2			/* sz*4 */
	addu	t6,t6,t0		/* t6: &(table[sz]) */
	lw	t5,0(t6)		/* t5: table[sz].p */

	li	v0,0xb000000		/* (sizeof(POLY_GT3)/4-1)<<24)+2 */
	sll	v1,t5,8			
	srl	v1,v1,8			/* v1: clear (31..24) of table[sz].p */
	or	v0,v0,v1		/* v0: (table[sz].p)| */
					/* (sizeof(POLY_GT3)/4-1)<<24) */
	sw	v0,0(t9) 		/* *(long *)pkt_base = v0 */
	
	.set noat
	li	AT,0xe2000000		/* tw reset */
	xor	v0,t9,t5		/* v0 : pkt_base xor table[sz].p */
	sw	AT,44(t9)		/* tw reset */
	.set at

	sll	v1,v0,8			
	srl	v0,v1,8			/* v0: clear (31..24) */
	xor	v1,v0,t5		/* v1: v0 xor table[sz].p */
	sw	v1,0(t6)		/* table[sz].p = pkt_base; */

	lw	v0,0(a3)		/* v0: primtop->tw */
	lw	t2,4(a3)		/* t2: primtop->uv0 */
	lw	t3,8(a3)		/* t3: primtop->uv1 */
	lw	t4,12(a3)		/* t4: primtop->uv2 */
	sw	v0,4(t9)		/* v0: primtop->tw */
	sw	t2,16(t9)		/* t2: primtop->uv0 */
	sw	t3,28(t9)		/* t3: primtop->uv1 */
	sw	t4,40(t9)		/* t4: primtop->uv2 */
        swc2    C2_SXY0,12(t9)		/* sxy0 -->8(pkt_base) */
        swc2    C2_SXY1,24(t9)		/* sxy1 -->20(pkt_base) */
	swc2    C2_SXY2,36(t9) 		/* sxy2 -->32(pkt_base) */
	swc2	C2_RGB0,8(t9)		/* R0,G0,B0,cd -->4(pkt_base) */
	swc2	C2_RGB1,20(t9)		/* R1,G1,B1,cd -->16(pkt_base) */
	swc2	C2_RGB2,32(t9)		/* R2,G2,B2,cd -->28(pkt_base) */
	addu	a3,a3,28		/* primtop++ */
	lw	t2,16(a3)		/* t2: primtop->v0,n0 */
	lw	t3,20(a3)		/* t3: primtop->v1,n1 */
	lw	t4,24(a3)		/* t4: primtop->v2,n2 */

	addu	t9,t9,48		/* pkt_base++; */
	subu	t8,t8,1
	bne	t8,0,TG3L_1L		/* if(primtop != primtop+n) goto TG3L_1 */
	nop
	b 	TG3L_3			
	nop
TG3L_2:

	addu	a3,a3,28		/* primtop++ */
	subu	t8,t8,1

	bne	t8,0,TG3L_1		/* if(primtop != primtop+n) goto TG3L_1 */
	nop
TG3L_3:
	lw	v0,0(a0)		/* sp->primp */
	sw	t9,GsOUT_PACKET_P
	addu	v0,v0,8			/* sp->primp+2 */

        .set    reorder
        j       ra			/* return(primtop) */
