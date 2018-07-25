/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsU_01000000

	
	.text
	

/************************************************************************
* Shared Calculate Vertex and Normal (GsUCAL)
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

GsU_01000000:

	.set	noreorder

	lw	t9,0(a0)		/* t9:sp->primp */
	lw	v1,24(a0)		/* v1:vertop */
	lw	t1,4(t9)		/* t1:number of shared vertex */
	lw	a1,8(t9)		/* a1:src offset of shared vertex */
	lw	t0,12(t9)		/* t0:dst offset of shared vertex */
	sll	v0,a1,3			/* src offset * 8 (2 WORD) */
	addu	a2,v0,v1		/* a2:vertop + src_offset */
	lw	v1,28(a0)		/* v1:vertop2 */
	sll	v0,t0,3			/* dst offset * 8 (2 WORD) */
	blez	t1,CAL_3		/* if (shared_vertex_num <= 0) goto CAL_3 */
	addu	a1,v0,v1		/* a1:vertop2 + dst_offset */
	lui	t3,0x8000		/* t3:0x80000000 */
	li	t2,-1			/* t2:0xffffffff */

	lw	a3,0(a2)		/* a3: vx,vy */
	lw	t0,4(a2)		/* t0: vz */
CAL_0:
	mtc2	a3,C2_VXY0		/* Vector 0 */
	mtc2	t0,C2_VZ0		
	nop	
	nop	

	/****/
	RTPS				
	/****/

	subu	t1,t1,1			/* t1-- */
	addiu	a2,a2,8			/* a2:a2+8(src_vertex++) */
	lw	a3,0(a2)		/* a3: vx,vy */
	lw	t0,4(a2)		/* t0: vz */

	cfc2	t4,C2_FLAG		/* t4: flag */
	nop	
	nop	
	and	t4,t4,t3		/* if ((flag & 0x80000000) == 0) */
	beq	t4,zero,CAL_1		/* goto CAL_1 */
	nop				/* else */
	j	CAL_2			/* goto CAL_2 */
	sw	t2,4(a1)		/* 0xffffff -> dst vertex area */
	
CAL_1:
	swc2	C2_SXY2,0(a1)		/* sx,sy	-> dst vertex area */
	mfc2	t8,C2_IR0		/* t8:p */
	mfc2	t7,C2_SZ3		/* t7:otz */
	sll	t8,t8,16		/* t8:(p << 16) */
	or	t8,t8,t7		/* t8:(p << 16) | otz */
	sw	t8,4(a1)		/* p,otz	-> dst vertex area */

CAL_2:
	bne	t1,zero,CAL_0		/* if (t1 != 0) goto CAL_0 */
	addiu	a1,a1,8			/* a1:a1+8(dst_vertex++) */

CAL_3:
	lw	t1,16(t9)		/* t1:number of shared normal */
	lw	a1,20(t9)		/* a1:src offset of shared normal */
	lw	t0,24(t9)		/* t0:dst offset of shared normal */
	lw	v1,32(a0)		/* v1:nortop */
	sll	v0,a1,3			/* v0:src_offset * 8 (2 WORD) */
	addu	a2,v0,v1		/* a2:nortop + src_offset */
	lw	v1,36(a0)		/* v1:nortop2 */
	sll	v0,t0,3			/* v0:dst_offset * 8 (2 WORD) */
	blez	t1,CAL_5		/* if (shared_normal_num <= 0) goto CAL_5 */
	addu	v1,v0,v1		/* v1:nortop2 + dst_offset */

	lw	a3,0(a2)		/* a3: nx,ny */
	lw	t0,4(a2)		/* t0: nz */
CAL_4:
	mtc2	a3,C2_VXY0		/* Normal 0 */
	mtc2	t0,C2_VZ0		
	nop	
	nop	

	/****/
	NCS				
	/****/

	subu	t1,t1,1			/* t1-- */
	addiu	a2,a2,8			/* a2:a2+8 */
	lw	a3,0(a2)		/* a3: nx,ny */
	lw	t0,4(a2)		/* t0: nz */

	mfc2	t4,C2_IR1		/* t4:LTr */
	mfc2	t5,C2_IR2		/* t5:LTg */
	mfc2	t6,C2_IR3		/* t6:LTb */

	sll	t5,t5,16		/* LTg << 16 */
	or	t5,t5,t4		/* (LTg << 16) | LTr */
	sw	t5,0(v1)		/* (LTg<<16)|LTr	-> dst normal area */
	sh	t6,4(v1)		/* LTb		-> dst normal area */

	bne	t1,zero,CAL_4		/* if (t1 != 0) goto CAL_4 */
	addiu	v1,v1,8			/* v1:v1+8 */

CAL_5:
	.set	reorder
	j	ra
	addu	v0,t9,28		/* sp->primp+7 */
