;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Z-buffer example						 ;
; (c) Sony Computer Entertainment Europe ;
; Stuart Ashley Nov '97					 ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	include "inline_a.h"
	opt 	at-
	opt 	c+
	opt		l.

px0		EQU		0		; POLY structure field x0
px1		EQU		8
px2		EQU		16
prgb	EQU		24
puv0	EQU		28
pclut	EQU		30
puv1	EQU		32
ptp		EQU		34
puv2	EQU		36
pot		EQU		38
psz 	EQU 	40		; POLY size
; to change the size of the z buffer zbincy needs changing and
; just before .zbloop the start and end address of the bounding box
; of the polygon needs to be changed
zbincy	EQU		640		; z buffer y increment value (320 shorts)
zbincx	EQU		4		; z buffer x increment value
sds		EQU		48		; size of stuff put on stack
pts		EQU		32		; size of polygon type structure (POLY_FT3)
pttag	EQU		0
ptrgb	EQU		4
ptx0	EQU		8
ptuv0	EQU		12
ptclut	EQU		14
ptx1	EQU		16
ptuv1	EQU		20
pttp	EQU		22
ptx2	EQU		24
ptuv2	EQU		28
ptot	EQU		30
ptz0	EQU		32
ptz1	EQU		36
ptz2	EQU		40
ptfz	EQU		44

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Macro to calculate max (o1) and min (o2) of 3 values (i1,i2,i3) ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
MAXMIN	macro	i1,i2,i3,o1,o2
	slt		at,		i2,		i1
	beq		at,		zero,	.l1\@
	slt		at,   	i3,	i1
	beq		at,		zero,	.l2\@
	slt		at,		i3,		i2
	beq		at,		zero,	.l3\@
	add		o1,		i1,		zero
	j		.l6\@
	add		o2,		i3,		zero
.l3\@
	j		.l6\@
	add		o2,		i2,		zero
.l2\@
	add		o1,		i3,		zero
	j		.l6\@
	add		o2,		i2,		zero
.l1\@
	slt		at, 	i3,		i2
	beq		at, 	zero,	.l4\@
	slt		at, 	i3,		i1
	beq		at, 	zero,	.l5\@
	add		o1,		i2,		zero
	j		.l6\@
	add		o2,		i3,		zero
.l5\@
	j		.l6\@
	add		o2,		i1,		zero
.l4\@
	add		o1,		i3,		zero
	add		o2,		i1,		zero
.l6\@
	endm


	section .text

	global	zdiv

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; compute the subdivision of the polygons and add them to the ot ;
; arg 1 (a0) - list of polys                                  	 ;
; arg 2 (a1) - zbuffer                                           ;
; arg 3 (a2) - ot											  	 ;
; arg 4 (a3) - list	of primatives								 ;
; arg 5 	 - number of polys									 ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
zdiv:
; save things
;;;;;;;;;;;;;
	subu	sp,		72				; move stack pointer
	sw		ra,		64(sp)			; store ra
	sw		t9,		60(sp)
	sw		t8,		56(sp)
	sw		s8,		52(sp)
	sw		s7,		48(sp)			; store s0..t7
	sw		s6,		44(sp)
	sw		s5,		40(sp)
	sw		s4,		36(sp)
	sw		s3,		32(sp)
	sw		s2,		28(sp)
	sw		s1,		24(sp)
	sw		s0,		20(sp)

	addi 	a0,		a0,		-psz

	; s8 - flat flag
	; v1 - count of polys on stack
.start
; get 3D polys turn into 2D polys
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	addi	a0,		a0,		psz		; move to next poly

	lw		at,		88(sp)			; arg 5, number of polygons
	nop

	blez	at,		.end			; finished all polys

	addi	at,		-1				; one less poly to do
	sw		at,		88(sp)

	lwc2	r0, 	0(a0)			; load vertices
	lwc2	r1, 	4(a0)
	lwc2	r2, 	8(a0)
	lwc2	r3, 	12(a0)
	lwc2	r4, 	16(a0)
	lwc2	r5, 	20(a0)

	nop
	nop
	RTPT							; perspective transformation

	nop
	nop
	NCLIP							; normal clip

	mfc2	at,		r24				; opz
	nop
	blez	at,		.start			; back facing poly

	add		v1,		zero,	zero	; number of stacked polys

; sort vertices
;;;;;;;;;;;;;;;;
	; t4..t6 - z0..z2
	; t7..t9 - y0x0..y2x2
	mfc2	t7,		r12				; screen x and y values
	mfc2	t8,		r13
	mfc2	t9,		r14

	mfc2	t4,		r17				; screen z values
	mfc2	t5,		r18
	mfc2	t6,		r19

	sra		t0,  	t7, 	$10  	; y0
	sra		t1,		t8, 	$10 	; y1
	sra    	t2, 	t9, 	$10 	; y2

	sub		s0, 	t0, 	t1		; y components of side lengths
	sub		s1, 	t1, 	t2
	sub		s2, 	t2, 	t0

	mtc2	s0, 	r9
	mtc2	s1, 	r10
	mtc2	s2, 	r11

	andi	s3, 	t7, 	$ffff	; x0
	andi	s4, 	t8, 	$ffff	; x1
	andi	s5, 	t9, 	$ffff	; x2

	;nop								
	;nop
	SQR		0						; square 3 values

	sub		s0, 	s3, 	s4		; x components of side lengths
	sub		s1, 	s4, 	s5
	sub		s2, 	s5, 	s3

	mfc2	t0,  	r25				; results of y^2
	mfc2	t1,  	r26
	mfc2	t2, 	r27

	mtc2	s0, 	r9
	mtc2	s1, 	r10
	mtc2	s2, 	r11

	nop								; square
	nop
	SQR		0

	mfc2	s3, 	r25				; results of x^2
	mfc2	s4, 	r26
	mfc2	s5, 	r27

	add		t0,  	t0,  	s3		; d0 (squares of lengths of sides)
	add		t1,  	t1,  	s4		; d1
	add		t2, 	t2, 	s5		; d2

	slt		at, 	t0, 	t1		; if (d0 < d1)

	beq		at, 	zero, 	.skp1
	slt		at, 	t2, 	t1		; if (d2 < d1)

	beq		at, 	zero, 	.skp1
	nop

	lh		s0,		puv0(a0)	   	; swap x,y and z so 0-1 longest side
	lh		s1,		puv1(a0)
	lh		s2,		puv2(a0)

	sh		s1,		puv0(a0)
	sh		s2,		puv1(a0)
	sh		s0,		puv2(a0)

	add		at,		t7,		zero
	add		t7,		t8,		zero
	add		t8,		t9,		zero
	add		t9,		at,		zero

	add		at,		t4,		zero
	add		t4,		t5,		zero
	add		t5,		t6,		zero
	j 		.skp2
	add		t6,		at,		zero

.skp1
	slt		at, 	t0, 	t2		; if (d0 < d2)
	beq		at, 	zero, 	.skp2

	slt		at, 	t1, 	t2		; if (d1 < d2)
	beq		at, 	zero,  	.skp2
	nop

	lh		s0,		puv0(a0)	   	; swap x,y and z so 0-1 longest side
	lh		s1,		puv1(a0)
	lh		s2,		puv2(a0)

	sh		s0,		puv1(a0)
	sh		s1,		puv2(a0)
	sh		s2,		puv0(a0)

	add		at,		t9,		zero
	add		t9,		t8,		zero
	add		t8,		t7,		zero
	add		t7,		at,		zero

	add		at,		t6,		zero
	add		t6,		t5,		zero
	add		t5,		t4,		zero
	add		t4,		at,		zero

.skp2

	beq		t7,		t8,		.next	; size check
	lui		s8,		$f000		   	; flat flag
	beq		t8,		t9,		.next	; size check
	nop
	beq		t9,		t7,		.next	; size check

.loop
; get max and min values
;;;;;;;;;;;;;;;;;;;;;;;;
	; s1,s2 - max and min x values for a poly
	; s3,s4 - max and min y values for a poly
	; s5,s6 - max and min z values for a poly

	andi	t0, 	t7, 	$ffff	; x0
	andi	t1, 	t8, 	$ffff	; x1
	andi	t2, 	t9, 	$ffff	; x2

	MAXMIN	t0,t1,t2,s1,s2			; find max and min x

	sra		t0, 	t7, 	$10		; get y0
	sra    	t1, 	t8, 	$10		; get y1
	sra    	t2, 	t9, 	$10		; get y2

	MAXMIN	t0,t1,t2,s3,s4			; find max and min y

	MAXMIN	t4,t5,t6,s5,s6			; find max and min z

; Calculate max and min z values for area of screen covered by poly
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; start max and min z s7 and s0
	add		s7,		s8,		zero
	lui		at,		$f000
	bne		s8,		at,		.forw
	add		s0,		s8,		zero

	add		s7,		zero, 	zero
	addi	s0,		zero, 	$7fff

	; t0 - start pointer into z buffer
	; t1 - pointer into z buffer
	; t2 - start of line pointer into z buffer
	; t3 - end pointer into z buffer

	; other max and min values
	addi	at,		zero,	-4
	and		s1,		s1,		at		; make stuff work in 2s
	and		s2,		s2,		at
	addi	at,		zero,	-2
	and		s3,		s3,		at
	and		s4,		s4,		at

	sll		at,		s3,		$08		; end address
	sll		t3,		s3,		$06
	add		t3,		t3,		at
	add		t3,		t3,		a1

	sll		at,		s4,		$08		; start address
	sll		t0,		s4,		$06
	add		t0,		t0,		at
	add		t0,		t0,		a1

	add		t2,		s2,		zero   	; t2, start x

	add		t1,		t0,		t2	   	; pointer into z-buffer

.zbloop								; z-buffer  loop
	lw		v0,		0(t1)		   	; load up next z value

	addi	t1,		t1,		zbincy 	; inc y

	sll		ra,		v0,		$10		; separate out the 2 z values
	sra		ra,		ra,		$10
	sra		v0,		v0,		$10

	slt		at,		s7,		v0		; start comparisons
	beq		at,		zero,	.chk1
	slt		at,		v0,		s0

	add		s7,		v0,		zero   	; new max z
.chk1
	beq		at,		zero,	.chk2
	slt		at,		s7,		ra

	add		s0,		v0,		zero   	; new min z
.chk2
	beq		at,		zero,	.chk3
	slt		at,		ra,		s0

	add		s7,		ra,		zero   	; new max z
.chk3
	beq		at,		zero,	.chk4
	slt		at,		t1,		t3	   	; end of y loop comparison

	add		s0,		ra,		zero   	; new min z
.chk4
	bne		at,		zero,	.zbloop
	nop

	addi	t2,		t2,		zbincx 	; inc x

	slt		at,		t2,		s1		; end of x loop comparison
	bne		at,		zero,	.zbloop
	add		t1,		t0,		t2	   	; update pointer into z-buffer

	sub		at, 	s7, 	s0	   	; check for flat(ish) z value
	addi 	at, 	at,		-4
	bgtz 	at, 	.forw

	slt		at,		s0,		s5

	add		s8,		zero,	s7		; store flat z value

.forw
	beq		at,		zero,	.addprim
	slt		at,		s6,		s7

	beq		at,		zero,	.next

	sub		at,		s1,		s2	   	; automatically add small polys to OT

	addi	at,		at,		-4	   	; max and min x are in half words

	bgtz	at,		.sub
	sub		at,		s3,		s4

	addi	at,		at,		-2

	bgtz	at,		.sub
	nop

; Add primative to ordering table
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.addprim
	lh		s3,		pot(a0)			; load OT value
	nop

	add		at,		a2,		s3

	lw		s0,		prgb(a0)		; load rgb
	lw		s1,		puv0(a0)		; load uv0 CLUT
	lw		s2,		puv1(a0)		; load uv1 TPAGE
	lh		s3,		puv2(a0)		; load uv2

	lw		t1,		0(at)		   	; ot pointer
	lui		t2,		$0700		   	; length for FT3
	add		t2,		t2,		t1
	sll		t3,		a3,		$08	   	; primative address
	srl		t3,		t3,		$08

	sw		t2,		pttag(a3)  		; primative pointer
	sw 		t3, 	0(a2)		   	; ot pointer

	sw		s0,		ptrgb(a3)		; save rgb
	sw		s1,		ptuv0(a3)		; save uv0 CLUT
	sw		s2,		ptuv1(a3)		; save uv1 TPAGE
	sh		s3,		ptuv2(a3)		; save uv2

	sw		t7,  	ptx0(a3) 		; save x0, y0
	sw		t8,  	ptx1(a3) 		; save x1, y1
	sw		t9, 	ptx2(a3) 		; save x2, y2

	j 		.next
	addi	a3,		a3,		pts		; increment ready for next addition

; Subdivide polygon into two
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.sub
	sra		t0,		t7,		$10		; y0
	sra		t1,		t8,		$10		; y1
	add		t2,		t0,		t1		; (y0 + y1 + 1) / 2
	addi	t2,		t2,		1
	sra		t2,		t2,		$01

	sll		v0,		t2,		$10

	andi	s5,		t7,		$ffff	; x0
	andi	s6,		t8,		$ffff	; x1
	add		s7,		s5,		s6		; (x0 + x1 + 1) / 2
	addi	s7,		s7,		1
	sra		s7,		s7,		$01

	or		v0,		v0,		s7

	beq		v0,		t9,		.next	; size check (neither poly has area)
	nop

	lh		s0,		puv0(a0)		; load u0,v0
	lh		s1,		puv1(a0)		; load u1,v1
	lh		s2,		puv2(a0)		; load u2,v2

	andi	s3,		s0,		$100	; calculate new u,v coordinate
	andi	at,		s1,		$100
	beq		s3,		at,		.nrnd
	add 	s3,		zero,	zero
	addi	s3,		zero,	$100
.nrnd
	add		s3,		s3,		s0
	add 	s3,		s3,		s1
	srl 	s3,		s3,		1

	addu	ra, 	t4, 	t5		; (z0 + z1 + 1) / 2
	addi	ra,		ra,		1
	beq		v0,		t7,		.miss1	; size check
	sra		ra, 	ra, 	$01		; finish calculating z in delay

	beq		v0,		t8,		.miss2	; size check of second poly
	nop

	subu	sp,		sds

	lw		s4,		prgb(a0)		; load rgb
	lh		s5,	  	pclut(a0)		; load clut
	lh		s6,	  	ptp(a0)			; load tpage
	lh		at,		pot(a0)			; load ot

	sw		s4,		ptrgb(sp)		; save rgb
	sh		s5,		ptclut(sp)		; save clut
	sh		s6,		pttp(sp)  		; save tpage
	sh		at,		ptot(sp)  		; save ot

	sw		t8,  	ptx0(sp)		; save x0, y0 (x1, y1)
	sw		t9,  	ptx1(sp)		; save x1, y1 (x2, y2)
	sw		v0, 	ptx2(sp)		; save x2, y2 ((x0+x1)/2, (y0+y1)/2)

	sw		s8,		ptfz(sp)		; store flatz value for poly 2

	sw		t5,  	ptz0(sp) 		; save z0 (z1)
   	sw		t6, 	ptz1(sp)		; save z1 (z2)
	sw		ra, 	ptz2(sp)		; save z2 ((z0+z1)/2)

	sh		s1,		ptuv0(sp)		; save u0,v0
	sh		s2,		ptuv1(sp)		; save u1,v1
	sh		s3,		ptuv2(sp)		; save u2,v2

	addi	v1,		v1,		1		; more moved onto stack
.miss2								; second poly has no area
	add		t8,		t7,		zero	; sort out order of xy0,xy1 and xy2
	add		t7,		t9,		zero
	add		t9,		v0,		zero

	add		t5,		t4,		zero	; sort out order of z0,z1 and z2
	add		t4,		t6,		zero
	add		t6,		ra,		zero

	sh		s2,		puv0(a0)		; save u0,v0
	sh		s0,		puv1(a0)		; save u1,v1
	sh		s3,		puv2(a0)		; save u2,v2

	j		.loop
	nop
	
.miss1								; first poly has no area
	beq		v0,		t8,		.next	; size check on second poly

	add		t7,		t8,		zero	; sort out order of xy0,xy1 and xy2
	add		t8,		t9,		zero
	add		t9,		v0,		zero

	add		t4,		t5,		zero	; sort out order of z0,z1 and z2
	add		t5,		t6,		zero
	add		t6,		ra,		zero

	sh		s1,		puv0(a0)		; save u0,v0
	sh		s2,		puv1(a0)		; save u1,v1
	sh		s3,		puv2(a0)		; save u2,v2

	j		.loop
	nop

; get next poly from stack
;;;;;;;;;;;;;;;;;;;;;;;;;;
.next
	addi	v1,		v1,		-1
	bltz	v1,		.start
	nop

	lw		t7,		ptx0(sp)		; load x0, y0 
	lw		t8,		ptx1(sp)		; load x1, y1 
	lw		t9,		ptx2(sp)		; load x2, y2 
	lw		t4,		ptz0(sp)		; load z0
	lw		t5,		ptz1(sp)		; load z1
	lw		t6,		ptz2(sp)		; load z2
	lw		s8,		ptfz(sp)		; load flatz value

	lw		t0,		ptrgb(sp)		; load rgb
	lw		t1,	  	ptuv0(sp)		; load u0,v0, clut
	lw		t2,	  	ptuv1(sp)		; load u1,v1, tpage
	lw		t3,		ptuv2(sp)		; load u2,v2. ot

	sw		t0,		prgb(a0)		; save rgb
	sw		t1,		puv0(a0)		; save u0,v0, clut
	sw		t2,		puv1(a0)		; save u1,v1, tpage
	sw		t3,		puv2(a0)		; save u2,v2, ot

	j 		.loop
	addu	sp,		sds

.end
; load things
;;;;;;;;;;;;;
	lw		s0,		20(sp)			; restore s0..s7
	lw		s1,		24(sp)
	lw		s2,		28(sp)
	lw		s3,		32(sp)
	lw		s4,		36(sp)
	lw		s5,		40(sp)
	lw		s6,		44(sp)
	lw		t7,		48(sp)
	lw		s8,		52(sp)
	lw		t8,		56(sp)
	lw		t9,		60(sp)
	lw		ra,		64(sp)			; restore ra

	addu	sp,		72				; restore stack

	j		ra						; return
	nop
