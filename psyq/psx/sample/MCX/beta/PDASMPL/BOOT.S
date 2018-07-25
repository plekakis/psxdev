;
; boot.S	general-purpose boot sequence
;

	GET	PERI.INC

        CODE32                          ; Subsequent instructions are ARM 
        AREA	BOOT, CODE, READONLY	; name this block of code

	IMPORT	IRQ_Callback
	IMPORT	FIQ_Callback
	IMPORT	main

;----------------------------------------------------------------------------
	GET	Mhead.S			; memory card file header
;----------------------------------------------------------------------------
        ENTRY                           ; program entry

	; comes from the startup application with the following settings:
	; user32 mode, 4 MHz clock, BATIRQ, RTCIRQ enabled, 
	; communication with the PS disabled
	; ÇÃèÛë‘Ç≈îÚÇÒÇ≈Ç≠ÇÈ
	; ÇÃèÛë‘Ç≈îÚÇÒÇ≈Ç≠ÇÈ
startup
	MOV	r0, #5			; first, set clock to 1 MHz
	SWI	4

	MOV	r0, #INTStatus		; set r0 to INTStatus address
	LDR	r1, =0x3fff
	STR	r1, [r0, #INTEnableReset_ofs]	; disable all IRQ, FIQ

	MOV	r0, #1			; use kernel system calls to
	LDR	r1, =IRQ_Callback	; register user IRQ, FIQ callbacks
	SWI	1
	MOV	r0, #2
	LDR	r1, =FIQ_Callback
	SWI	1

	MOV	r0, #INTStatus
	MOV	r1, #0x0a00
	STR	r1, [r0, #INTEnableSet_ofs]	; enable BAT, RTC

	MOV	r0, #0x200
	MOV	r1, #0
01
	STR	r1, [r0], #4		; clear user RAM region
	CMP	r0, #0x800
	BNE	%B01

	MOV	sp, #0x00000800		; initial SP value in user mode 0x800
	B	main			; to main routine

;----------------------------------------------------------------------------
	END
