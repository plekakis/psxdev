;
; KEY.S		key entry functions
;

	GET	PERI.INC
	GET	STRUCT.INC

	EXPORT	key_params
	EXPORT	KEY_Init
	EXPORT	KEY_Read
	EXPORT	KEY_AbortCheck

;=================================================================
        AREA	KEY_BSS, DATA
key_params	%	6		; key parameter
;=================================================================
	CODE16
        AREA	LIB_KEY, CODE, READONLY, INTERWORK
;-----------------------------------------------------------------
; function KEY_Init
; CODE16
; key initialize
; Parameters: r0 contents updated each r0 increments of counter (receive continuous input)
;          if r0=0, no continuous input
;	r1 counter for checking forced exit via ENTER key
;	   if r1=0, no checking is performed
; Return value: new key entry
; Registers used: r0-1
;-----------------------------------------------------------------
KEY_Init
	PUSH	{ r7 }
	PUSH	{ lr }
	LDR	r7, =key_params		; r7 key_params address
	STRB	r1, key_abort_val	; abort initial value store
	MOV	r1, #0
	STRB	r1, key_abort_cnt	; abort counter store

	STRB	r0, key_times
	STRB	r0, key_timescnt
	MOV	r0, #0
	STRB	r0, key_buf		; key_buf=0
	MOV	r0, #0x3f
	STRB	r0, key_buf_old		; key_buf_old=0x3f

	MOV	r0, #INTStatus_r20	; enable EXTIRQ 0-5 
	LSL	r0, r0, #20
	MOV	r1, #0x3f
	STR	r1, [r0, #INTEnableSet_ofs]

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r7 }
	BX	lr

;-----------------------------------------------------------------
; function KEY_Read
; CODE16
; return key entry
; parameters: none
; Return value: r0 button (EXTIRQ) entry (bit[0-5])
; Registers used: r1-2
;-----------------------------------------------------------------
KEY_Read
	PUSH	{ r1-r3, r7 }
	PUSH	{ lr }
	LDR	r7, =key_params		; r7 key_params address
	LDRB	r0, key_buf		; r0 key_buf contents
	MOV	r3, r0			; copy to r3
	LDRB	r1, key_buf_old		; r1 key_buf_old contents
	STRB	r0, key_buf_old		; save r0 
	EOR	r1, r0			; r1 result of EOR of r0 and r1
	AND	r0, r1			; r0 newly pressed button
	BNE	%F02			; 02 if new key entry is present

	; if new key entry is not present (r0=0)
	LDRB	r2, key_timescnt	; r2 counter value
	CMP	r2, #0
	BEQ	%F01			; if 0, unconditionally 01

	; if r0=0 and counter is not 0
	SUB	r2, #1
	STRB	r2, key_timescnt
	CMP	r2, #0
	BNE	%F01			; if has not reached 0, 01
	MOV	r0, r3			; if 0 reached, return entry directly
02
	LDRB	r1, key_times
	STRB	r1, key_timescnt	; reset key_timescnt
01
	MOV	r2, #0
	STRB	r2, key_buf		; clear key_buf

	MOV	r1, #INTStatus_r20	; enable EXTIRQ 0-5 
	LSL	r1, r1, #20
	MOV	r2, #0x3f
	STR	r2, [r1, #INTEnableSet_ofs]

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r1-r3, r7 }
	BX	lr

;-----------------------------------------------------------------
; function KEY_AbortCheck
; CODE16
; monitor ENTER key
; parameters: none
; Return value: r0	1: ENTER key was pressed continuously for abort_val
;		otherwise, 0
; Registers used: r1-2,r7
;-----------------------------------------------------------------
KEY_AbortCheck
	PUSH	{ r1, r2, r7 }
	PUSH	{ lr }

	LDR	r7, =key_params		; r7 key_params address
	LDRB	r0, key_abort_val
	CMP	r0, #0
	BEQ	%F01			; return if key_abort_val is 0

	MOV	r0, #INTStatus_r20
	LSL	r0, r0, #20
	LDR	r1, [r0, #INTRawStatus_ofs]
	MOV	r2, #0x01
	TST	r1, r2
	BNE	%F04	
	MOV	r0, #0			; if not pressed, return value 0
	B	%F02			; to 02
04
	LDRB	r2, key_abort_cnt
	LDRB	r1, key_abort_val
	CMP	r2, r1
	BNE	%F03
	MOV	r0, #1			; if reached key_abort_val, return value 1
	B	%F02			; to 02
03
	ADD	r2, r2, #1		; if key_abort_val was not reached, 
	STRB	r2, key_abort_cnt	; increment and return 0
	MOV	r0, #0
	B	%F01			; to 01
02
	MOV	r2, #0
	STRB	r2, key_abort_cnt	; set abort_cnt to 0
01
	POP	{ r7 }
	MOV	lr, r7
	POP	{ r1, r2, r7 }
	BX	lr

;-----------------------------------------------------------------
	END
