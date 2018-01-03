;
; Timer.S	timer functions
;

	GET	PERI.INC
	GET	STRUCT.INC

	EXPORT	timer_params
	EXPORT	TIMER_Init
	EXPORT	TIMER_Stop
	EXPORT	TIMER_Start
	EXPORT	TIMER_Wait

;=================================================================
        AREA	KEY_BSS, DATA
timer_params	%	6		; timer parameter
;=================================================================
; set up INTERWORK attributes since this is called from ARM Code.
; functions return with BX, in accordance with the ARM Procedure Call Standard (APCS).
; When linking, "veneer code", which uses r12, is inserted
; so that the user cannot use r12. (APCS specifications: warning)
	CODE16
        AREA	LIB_TIMER, CODE, READONLY, INTERWORK
;-----------------------------------------------------------------
; function TIMER_Init
; CODE16
; initialize timer (set parameters, enable interrupt)
; Parameters: r0  timer number
;	r1  TimerLoad (16bit)
;       r2  CLKDIV (2bit)
; Return value: none
; Registers used: r4-7
;-----------------------------------------------------------------
TIMER_Init
	PUSH	{ r4-r7 }
	PUSH	{ lr }
	MOV	r5, r0			; save r0
	BL	TIMER_Stop		; must be stopped temporarily
	MOV	r0, r5

	LDR	r7, =timer_params	; initialize timer buffers 0-2
	LSL	r5, #1
	ADD	r5, r7, #2
	MOV	r6, #0
	STRB	r6, [r5]		; timerX_buf=0
	STRB	r6, [r5, #1]		; timerX_buf_old=0

	MOV	r4, #INTStatus_r20
	LSL	r4, r4, #20
	MOV	r6, #0x80
	CMP	r0, #0
	BEQ	%F01
	LSL	r6, #1
	CMP	r0, #1
	BEQ	%F01
	LSL	r6, #5
01
	STR	r6, [r4, #INTEnableSet_ofs]	; enable timer interrupt

	MOV	r4, #TIMER0Load_r20
	LSL	r4, r4, #20
	LSL	r0, r0, #4
	ADD	r4, r4, r0
	STR	r1, [r4]
	MOV	r6, #0x04
	BIC	r2, r6				; do not start yet
	STR	r2, [r4, #TIMER0Control_ofs]	; enable timer

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r4-r7 }
	BX	lr

;-----------------------------------------------------------------
; function TIMER_Start
; CODE16
; start timer
; Parameters: r0  timer number
; Return value: none
; Registers used: r5-r7
;-----------------------------------------------------------------
TIMER_Start
	PUSH	{ r5-r7 }
	PUSH	{ lr }
	MOV	r7, #TIMER0Load_r20
	LSL	r7, r7, #20
	LSL	r0, r0, #4
	ADD	r7, r7, r0
	LDR	r6, [r7, #TIMER0Control_ofs]
	MOV	r5, #0x04
	ORR	r6, r5
	STR	r6, [r7, #TIMER0Control_ofs]	; stop timer

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r5-r7 }
	BX	lr

;-----------------------------------------------------------------
; function TIMER_Stop
; CODE16
; stop timer
; Parameters: r0  timer number
; Return value: none
; Registers used: r5-r7
;-----------------------------------------------------------------
TIMER_Stop
	PUSH	{ r5-r7 }
	PUSH	{ lr }
	MOV	r7, #TIMER0Load_r20
	LSL	r7, r7, #20
	LSL	r0, r0, #4
	ADD	r7, r7, r0
	LDR	r6, [r7, #TIMER0Control_ofs]
	MOV	r5, #0x04
	BIC	r6, r5
	STR	r6, [r7, #TIMER0Control_ofs]	; stop timer

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r5-r7 }
	BX	lr

;-----------------------------------------------------------------
; function TIMER_Wait
; CODE16
; wait until timer interrupt takes place
; Parameters: r0 timer number
; Return value:r0 number of times interrupt occurred since last time
; Registers used: r4-7
;-----------------------------------------------------------------
TIMER_Wait
	PUSH	{ r4-r7 }
	PUSH	{ lr }
	LDR	r7, =timer_params
	LSL	r0, #1
	ADD	r0, r7, r0
	LDRB	r5, [r0]		; timerX_buf
	LDRB	r6, [r0, #1]		; timerX_buf_old
	SUB	r6, r5, r6		; r6 difference
01
	LDRB	r4, [r0]		; timerX_buf
	CMP	r5, r4
	BEQ	%B01
	STRB	r4, [r0, #1]		; old update
	MOV	r0, r6

	POP	{ r7 }
	MOV	lr, r7
	POP	{ r4-r7 }
	BX	lr

;-----------------------------------------------------------------
	END

