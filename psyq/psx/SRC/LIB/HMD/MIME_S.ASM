/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <r3000.h>
#include <asm.h>
#include <inline_s.h>
#include <gtereg_s.h>

	.globl	GsVNMIMeFunc
	.globl	GsRestoreOrgsVNMIMe

	.text


/* void	GsVNMIMeFunc(SVECTOR *vec, SVECTOR *dvec, long np, long weight); */

GsVNMIMeFunc:
	mtc2	a3, C2_IR0	/* C2_IR0 = weight */

loop_vn_mime:
	lw	t3, 0(a1)	/* t3 = dvec->vx, dvec->vy */
	lh	t5, 4(a1)	/* t5 = dvec->vz */
	sra	t4, t3, 16	/* t4 = dvec->vy */
	/* and	t3, t3, 0xffff	/* t3 = dvec->vx */
	.set noreorder
	mtc2	t3, C2_IR1
	mtc2	t4, C2_IR2
	mtc2	t5, C2_IR3
	add	a1, a1, 8	/* dvec++ */
	sub	a2, a2, 1	/* np--; */
	GPF(1)
	lw	t0, 0(a0)	/* t0 = vec->vx, vec->vy */
	lh	t2, 4(a0)	/* t2 = vec->vz */
	sra	t1, t0, 16	/* t1 = vec->vy */
	/* and	t0, t0, 0xffff	/* t0 = vec->vx */
	mfc2	t3, C2_MAC1
	mfc2	t4, C2_MAC2
	mfc2	t5, C2_MAC3
	.set reorder
	add	t3, t0, t3	/* t3 = vec->vx + MAC1 */
	add	t4, t1, t4	/* t4 = vec->vy + MAC2 */
	add	t5, t2, t5	/* t5 = vec->vz + MAC3 */
	sh	t3, 0(a0)	/* vec->vx = t3 */
	sh	t4, 2(a0)	/* vec->vy = t4 */
	sh	t5, 4(a0)	/* vec->vz = t5 */
	add	a0, a0, 8	/* vec++ */
	bgtz	a2, loop_vn_mime/* if (np > 0) goto loop_vn_mime */

	/* move	v0, a1		/* return dvec (next addr) */
	j	ra


/* void	GsRestoreOrgsVNMIMe(RstVNMIMePrim *pPrim,
	u_long *pDiffSect, SVECTOR *orgs, SVECTOR *vn); */


#define pPrim		a0
#define pDiffSect	a1
#define orgs		a2
#define vn		a3

#define n		t0
#define idxDiffp	t1
#define pDiff		t2
#define j		t3
#define	pData		t4
#define k		t5
#define vnp		t6
#define orgsp		t7

#define tmp1		v0
#define tmp2		v1

GsRestoreOrgsVNMIMe:

	/* the SCANing flag is already 0 at this point */
	lw	n, 0(pPrim)		/* n = pPrim->nSize, n */
	add	idxDiffp, pPrim, 4	/* idxDiffp = &pPrim->idxDiff[0] */
	sra	n, n, 16		/* n = nPrim->n */

loop_n:
	beq	n, zero, ret

	lw	tmp1, 0(idxDiffp)	/* idxDiff = *idxDiffp */
	add	idxDiffp, idxDiffp, 4	/* idxDiffp++ */
	sub	n, n, 1			/* n-- */
	sll	tmp1, tmp1, 2		/* idxDiff *= sizeof(pDiffSect[0]) (== 4) */
	add	pDiff, pDiffSect, tmp1	/* pDiff = &pDiffSect[idxDiff] */
	sub	pData, pDiff, 12-4	/* pData = &pDiff->diffData[-1] */
	lw	j, 0(pDiff)		/* j = pDiff->nZero, nDiffsNum */
	sra	j, j, 16		/* j = pDiff->nDiffsNum */

loop_j:
	beq	j, zero, loop_n

	add	pData, pData, 12	/* pData++ */
	sub	j, j, 1			/* j-- */
	lw	tmp1, 8(pData)		/* tmp = pData->nChanged, nVnum */
	and	tmp2, tmp1, 0x0ffff	/* tmp = pData->nChanged */
	beq	tmp2, zero, loop_j

	swl	zero, 8(pData)		/* pData->nChanged = 0 */
	sra	k, tmp1, 16		/* k = pData->vNum */

	lw	tmp1, 0(pData)		/* vs = pData->idxVstart */
	lw	tmp2, 4(pData)		/* os = pData->idxOstart */
	sll	tmp1, tmp1, 3		/* vs *= sizeof(SVECTOR) (== 8) */
	sll	tmp2, tmp2, 3		/* os *= sizeof(SVECTOR) (== 8) */
	add	vnp, vn, tmp1		/* vnp = &((u_long *)vn)[vs] */
	add	orgsp, orgs, tmp2	/* orgsp = &((u_long *)orgs)[os] */

loop_k:
	beq	k, zero, loop_n

	sub	k, k, 1			/* k-- */

	lw	tmp1, 0(orgsp)
	lw	tmp2, 4(orgsp)
	add	orgsp, orgsp, 8		/* orgsp++ */
	sw	tmp1, 0(vnp)
	sw	tmp2, 4(vnp)
	add	vnp, vnp, 8		/* vnp++ */

#undef j
	j	loop_k

ret:
	j	ra
