;
; CBACK.S	user callback functions
;

	GET	PERI.INC
	GET	STRUCT.INC

	EXPORT	IRQ_Callback
	EXPORT	FIQ_Callback
	IMPORT	key_params
	IMPORT	timer_params
	IMPORT	DAC_Dataset

;========================================================================
	CODE32
        AREA	CBACK, CODE, READONLY
;========================================================================
; parent routine
;------------------------------------------------------------------------
; IRQ callback
;------------------------------------------------------------------------
	; comes from kernel in mode IRQ32
IRQ_Callback
	STMDB	sp!, {r0-r3, lr}	; minimum save to stack
	MOV	r1, #INTStatus		; set r1 to INTStatus address
	LDR	r0, [r1]		; set r0 to INTStatus contents
	LDR	r3, [r1, #INTEnableSet_ofs]	; change from Rev.G
	AND	r2, r0, r3		; take AND with Enable and put in r2
	ADR	r3, IRQ_Branch
01
	MOVS	r2, r2, LSR #1
	MOVCS	lr, pc			; if C flag is 1
	LDRCS	pc, [r3]		; to all routines
	ADD	r3, r3, #4
	CMP	r2, #0
	BNE	%B01			; if remaining, 01
	LDMIA	sp!, {r0-r3, pc}	; return to kernel temporarily. Then
					; a resume operation is performed from an interrupt.
IRQ_Branch
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_EXT
	DCD	IRQ_Callback_SPI	; FIQ, so should not be called here
	DCD	IRQ_Callback_TC0
	DCD	IRQ_Callback_TC1
	DCD	IRQ_Callback_RTC
	DCD	IRQ_Callback_VOL
	DCD	IRQ_Callback_BAT
	DCD	IRQ_Callback_IF
	DCD	IRQ_Callback_TC2	; FIQ, so should not be called here

;------------------------------------------------------------------------
; FIQ callback
;------------------------------------------------------------------------
	; comes from kernel in mode FIQ32
FIQ_Callback
	STMDB	sp!, {r0-r1, lr}	; r8-r12 are reserved for FIQ so are not saved
	MOV	r1, #INTStatus		; set r1 to INTStatus address
	LDR	r0, [r1]		; set r0 to INTStatus contents
	LDR	r8, [r1, #INTEnableSet_ofs]	; change from Rev.G
	AND	r9, r0, r8		; take AND with Enable and put in r9
	MOV	r0, r9			; copy to r0
	TST	r9, #0x0040
	BLNE	FIQ_Callback_SPI
	TST	r9, #0x2000
	BLNE	FIQ_Callback_TC2
	LDMIA	sp!, {r0-r1, pc}	; return to kernel region for now. Then
					; a resume operation is performed from an interrupt.

;========================================================================
; IRQ handling routines
;------------------------------------------------------------------------
; function IRQ_Callback_EXT
; CODE32
; IRQ callback EXT
; Parameters: r0 INTStatus & INTEnableSet
;       r1 address of INTStatus
; Return value: none
; Registers used: r4-6
;------------------------------------------------------------------------
IRQ_Callback_EXT
	STMDB	sp!, {r4-r5, r7, lr}
	AND	r5, r0, #0x3f
	LDR	r7, =key_params
	LDRB	r4, key_buf
	ORR	r4, r4, r5		; take OR
	STRB	r4, key_buf

	MOV	r4, #INTStatus
	STR	r5, [r4, #INTEnableReset_ofs]	; button that was pressed (EXTIRQ)
						; Disable
	LDMIA	sp!, {r4-r5, r7, pc}	; return

;-----------------------------------------------------------------
; function IRQ_Callback_TC0
; CODE32
; IRQ callback TIMER0
; Parameters: r0 INTStatus & INTEnableSet
;       r1 address of INTStatus
; Return value: none
; Registers used: 
;-----------------------------------------------------------------
IRQ_Callback_TC0
	STMDB	sp!, {r6-r7, lr}
	LDR	r7, =timer_params
	LDRB	r6, timer0_buf
	ADD	r6, r6, #1		; increment timer 0 buffer
	STRB	r6, timer0_buf

	MOV	r6, #0x80
	STR	r6, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r6-r7, pc}	; return

;------------------------------------------------------------------------
; IRQ callback, etc.
; Parameters: r0 INTStatus & INTEnableSet
;       r1 address of INTStatus
; Return value: none
;------------------------------------------------------------------------
IRQ_Callback_TC1
	STMDB	sp!, {r6, lr}
	MOV	r6, #0x0100
	STR	r6, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r6, pc}

IRQ_Callback_IF
	STMDB	sp!, {r6, lr}
	MOV	r6, #0x1000
	STR	r6, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r6, pc}

IRQ_Callback_VOL
	STMDB	sp!, {r6, lr}
	MOV	r6, #0x0400
	STR	r6, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r6, pc}

	; RTCIRQ, BATIRQ must not be cleared here since
	; kernel operations will take place later
IRQ_Callback_RTC
IRQ_Callback_BAT
	MOV	pc, lr

	; the following is for FIQ, so should not come here
IRQ_Callback_SPI
IRQ_Callback_TC2
	MOV	pc, lr

;========================================================================
; FIQ handling routines
;------------------------------------------------------------------------
; function FIQ_Callback_TC2
; CODE32
; FIQ callback TIMER2
; Parameters: r0 INTStatus & INTEnableSet
;       r1 address of INTStatus
; Return value: none
; Registers used: r8
;------------------------------------------------------------------------
FIQ_Callback_TC2
	STMDB	sp!, {r8, lr}

	BL	DAC_Dataset			; set data to DAC

	MOV	r8, #0x00002000			; TC2FIQ
	STR	r8, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r8, pc}

;------------------------------------------------------------------------
; FIQ callback, etc.
; Parameters: r0 INTStatus & INTEnableSet
;       r1 address of INTStatus
; Return value: none
;------------------------------------------------------------------------
	; basically, kernel will handle so not needed (as of 10/1/98)
FIQ_Callback_SPI
	STMDB	sp!, {r6, lr}
	MOV	r6, #0x0040			; SPIFIQ
	STR	r6, [r1, #INTSourceClear_ofs]	; edge-based, so clear
	LDMIA	sp!, {r6, pc}

;------------------------------------------------------------------------
	END
