;
; Warning: Do not use this function!!!
; It may not work with future Playstation hardware.
;
	opt	c+
	opt	at-

BIU	equ	$FFFE0130	;BIU/cache config register
bit16	equ	1<<16
bit15	equ	%1000000000000000
bit14	equ	%0100000000000000
bit13	equ	%0010000000000000
bit12	equ	%0001000000000000
bit11	equ	%0000100000000000
bit10	equ	%0000010000000000
bit9	equ	%0000001000000000
bit8	equ	%0000000100000000
bit7	equ	%0000000010000000
bit6	equ	%0000000001000000
bit5	equ	%0000000000100000
bit4	equ	%0000000000010000
bit3	equ	%0000000000001000
bit2	equ	%0000000000000100
bit1	equ	%0000000000000010
bit0	equ	%0000000000000001

;
; position independent (thus relocatable) la macro
;
lea	macro	dest,addr
	move	r1,r31		;must save r31 before branch
	bal	@_pc\@		;sets r31 right away so...
	addi	\dest,r31,\addr-@_pc\@	;this *will* work in delay slot
@_pc\@	move	r31,r1		;and finally restore r31
	endm

	section	.text

; void SNFlushCache()

	xdef	SNFlushCache

SNFlushCache
	mfc0	t4,status
	li	at,~3
	and	at,at,t4	;all ints off
	mtc0	at,status

;	lea	t0,@inval	;PC relative address load
;	lui	at,$A000	;force into uncached address space
;	or	t0,t0,at

	la	t0,@inval|$A0000000

	jr	t0
	nop
;
; This routine to invalidate the instruction cache must be called
; in uncached space (kseg1) with ints off
;
@inval	li	t1,BIU
	lw	t3,0(t1)	;get BIU/cache config reg

	li	at,~(bit10+bit7+bit1+bit0)
	and	at,at,t3	;IS0,DS,INV,LOCK = 0
	ori	at,at,bit11+bit2	;IS1,TAG = 1 (tag test mode)
	sw	at,0(t1)

	mfc0	t2,status
	li	at,1<<16	;(SR bit 16 = isolate cache)
	or	at,at,t2
	mtc0	at,status
	nop
	nop

; in "tag test mode" - clear all tag entries
	li	at,0
	li	t0,$0f80
@nxt	sw	zero,$00(at)
	sw	zero,$10(at)
	sw	zero,$20(at)
	sw	zero,$30(at)
	sw	zero,$40(at)
	sw	zero,$50(at)
	sw	zero,$60(at)
	sw	zero,$70(at)
	bne	at,t0,@nxt
	addiu	at,at,$80

	li	at,~bit16
	and	at,at,t2
	mtc0	t2,status	;un-isolate the cache
	nop
	nop

	sw	t3,0(t1)	;restore BIU/cache config reg
	nop
	nop
	mtc0	t4,status	;and restore int status

	jr	ra
	nop

	end

