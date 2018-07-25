;
; MACRO.S	various macros
;

;-----------------------------------------------------------------
; macro F_LDRB
; for CODE16
; load byte by byte from FLASH. do not specify r0, r1 for Rd, Rn.
; registers used:r0, r1
;-----------------------------------------------------------------
	CODE16
	MACRO
	F_LDRB	$Rd, $Rn
	PUSH	{ r0,r1 }
	MOV	r1, $Rn
	MOV	r0, #0x01
	BIC	r1, r0
	LDRH	$Rd, [r1]
	AND	r0, $Rn
	BNE	%F90
	LSL	$Rd, #24		; Little Endian, so remove high order
	LSR	$Rd, #24
	B	%F91
90
	LSR	$Rd, #8
91
	POP	{ r0,r1 }
	MEND

;-----------------------------------------------------------------
; macro F_LDRB32
; for CODE32
; load byte by byte from FLASH. do not specify r0 for Rd, Rn.
; registers used: r0
;-----------------------------------------------------------------
	CODE32
	MACRO
	F_LDRB32	$Rd, $Rn
	STMDB	sp!, { r0 }
	BIC	r0, $Rn, #0x01
	LDRH	$Rd, [r0]
	TST	$Rn, #0x01
	MOVEQ	$Rd, $Rd, LSL #24	; Little Endian, so remove high order
	MOVEQ	$Rd, $Rd, LSR #24
	MOVNE	$Rd, $Rd, LSR #8
	LDMIA	sp!, { r0 }
	MEND

;-----------------------------------------------------------------
	END

