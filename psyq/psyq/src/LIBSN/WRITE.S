	.file	1 "write.c"

 # GNU C 2.6.3 [AL 1.1, MM 40] Sony Playstation compiled by GNU C

 # Cc1 defaults:
 # -mgas -msoft-float

 # Cc1 arguments (-G value = 8, Cpu = 3000, ISA = 1):
 # -quiet -O2 -o

gcc2_compiled.:
__gnu_compiled_c:
	.text
	.align	2
	.globl	PCwrite

	.text
	.ent	PCwrite
PCwrite:
	.frame	$sp,48,$31		# vars= 0, regs= 8/0, args= 16, extra= 0
	.mask	0x807f0000,-4
	.fmask	0x00000000,0
	subu	$sp,$sp,48
	sw	$20,32($sp)
	move	$20,$4
	sw	$19,28($sp)
	move	$19,$5
	sw	$16,16($sp)
	move	$16,$6
	sw	$18,24($sp)
	move	$18,$0
	sw	$31,44($sp)
	sw	$22,40($sp)
	sw	$21,36($sp)
	.set	noreorder
	.set	nomacro
	beq	$16,$0,$L3
	sw	$17,20($sp)
	.set	macro
	.set	reorder

	li	$22,0x00008000		# 32768
	li	$21,-1			# 0xffffffff
	sltu	$2,$22,$16
$L11:
	.set	noreorder
	.set	nomacro
	beq	$2,$0,$L5
	move	$17,$16
	.set	macro
	.set	reorder

	li	$17,0x00008000		# 32768
$L5:
	move	$4,$0
	move	$5,$20
	move	$6,$17
	.set	noreorder
	.set	nomacro
	jal	_SN_write
	move	$7,$19
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	bne	$2,$21,$L7
	addu	$18,$18,$2
	.set	macro
	.set	reorder

	.set	noreorder
	.set	nomacro
	j	$L10
	li	$2,-1			# 0xffffffff
	.set	macro
	.set	reorder

$L7:
	addu	$19,$19,$2
	subu	$16,$16,$2
	slt	$2,$2,$17
	bne	$2,$0,$L3
	.set	noreorder
	.set	nomacro
	bne	$16,$0,$L11
	sltu	$2,$22,$16
	.set	macro
	.set	reorder

$L3:
	move	$2,$18
$L10:
	lw	$31,44($sp)
	lw	$22,40($sp)
	lw	$21,36($sp)
	lw	$20,32($sp)
	lw	$19,28($sp)
	lw	$18,24($sp)
	lw	$17,20($sp)
	lw	$16,16($sp)
	addu	$sp,$sp,48
	j	$31
	.end	PCwrite
