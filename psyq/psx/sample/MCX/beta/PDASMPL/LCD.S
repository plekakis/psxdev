;
; LCD.S		LCD functions
;

	GET	PERI.INC
	GET	MACRO.INC

	IMPORT	fontdata88
	EXPORT	LCD_Init
	EXPORT	LCD_Clear
	EXPORT	LCD_print

;=================================================================
; set up INTERWORK attributes since this is called from ARM Code.
; functions return with BX, in accordance with the ARM Procedure Call Standard (APCS).
; When linking, "veneer code", which uses r12, is inserted
; so that the user cannot use r12. (APCS specifications: warning)
	CODE16
        AREA	LIB_LCD, CODE, READONLY, INTERWORK
;-----------------------------------------------------------------
; function LCD_Init
; CODE16
; initialize LCD
; Parameters: r0 frame rate 0: disable 1: 64Hz 2: 32Hz 3: 16Hz
;       r1 mode 0: full-screen display
; Return value: none
; Registers used: r3-5
;-----------------------------------------------------------------
LCD_Init
	PUSH	{ r3-r5 }
	PUSH	{ lr }
	MOV	r5, #LCDControl_r20
	LSL	r5, #20
	LDR	r4, [r5]		; r4 current LCDControl contents
	MOV	r3, #0x7f
	BIC	r4, r3			; everything except ROT will be set here, so clear

	ORR	r4, r1			; display region
	LSL	r0, #4
	ORR	r4, r0			; frame rate
	MOV	r0, #0x48
	ORR	r4, r0			; charge pump ON, display ON
	STR	r4, [r5]

	MOV	r5, #PIOControl_r20
	LSL	r5, #20			; band gap, voltage comparator ACTIVE
 	MOV	r4, #0x00		; voltage periodic test mode OFF
	STR	r4, [r5, #LVDControl_ofs]

	POP	{ r3 }
	MOV	lr, r3
	POP	{ r3-r5 }
	BX	lr

;-----------------------------------------------------------------
; function LCD_Clear
; CODE16
; screen clear (paint with dither pattern r0[7:0])
; Parameters: r0
; Return value: none
; Registers used: r1-r3
;-----------------------------------------------------------------
LCD_Clear
pat	RN	0
tmp1	RN	1
i	RN	2
BPaddr	RN	3
	PUSH	{ r1-r3 }
	PUSH	{ lr }
	LDR	BPaddr, =BackPlane0	; get BPaddr

	LSL	tmp1, pat, #8		; create 32bit data
	ORR	pat, tmp1
	LSL	tmp1, pat, #16
	ORR	pat, tmp1

	MOV	i, #32			; y = 1-32
01
	STR	pat, [BPaddr]
	MOV	tmp1, #1
	ROR	pat, tmp1		; offset pattern by 1 bit
	ADD	BPaddr, #4
	SUB	i, #1
	BNE	%B01

	POP	{ r1 }
	MOV	lr, r1
	POP	{ r1-r3 }
	BX	lr

;-----------------------------------------------------------------
; function LCD_print
; CODE16
; display string (8x8)
; Parameters: r0  X (in dots)
;	r1  Y (in dots)
;	r2  starting address of string
; Return value: none
; Registers used: r0-7, r8, r9
;-----------------------------------------------------------------
XP	RN	0
YP	RN	1
tmp3	RN	2
BPaddr1	RN	3
mask	RN	4
fptr	RN	5
i1	RN	6
tmp2	RN	7
U_Xstart	RN	8
U_CHaddr	RN	9

LCD_print
	PUSH	{ r3-r7 }
	PUSH	{ lr }
	MOV	r3, r8			; r8, 9 cannot be saved with PUSH
	MOV	r4, r9			; so do separately
	PUSH	{ r3-r4 }

	MOV	U_Xstart, r0		; save initial value of X in high-order register
	MOV	U_CHaddr, r2		; r2 will be used as tmp3 so save
04
	MOV	tmp3, U_CHaddr
	F_LDRB	tmp2, tmp3		; macro F_LDRB (byte load from FLASH)
	CMP	tmp2, #0
	BEQ	%F03			; stop if null character
	CMP	tmp2, #0x0A
	BNE	%F02
	ADD	YP, #8			; if linefeed, move Y, restore X and go to next character
	MOV	XP, U_Xstart
	MOV	tmp2, #1
	ADD	U_CHaddr, tmp2		; next character
	B	%B04
02
	LDR	fptr, =fontdata88	; detect font data address
	SUB	tmp2, #0x20
	LSL	tmp2, #3
	ADD	fptr, fptr, tmp2	; fptr= relevant font data address

	MOV	mask, #0xff
	LSL	mask, XP		; mask generate mask

	LDR	BPaddr1, =BackPlane0    ; get BPaddr
	LSL	tmp2, YP, #2		; 
	ADD	BPaddr1, tmp2		; BPaddr1= relevant address in BackPlane

	MOV	i1, #8
01
	LDR	tmp2, [BPaddr1]		; tmp2= data for current line
	BIC	tmp2, mask		; erase section to be written to
	F_LDRB	tmp3, fptr 		; tmp3= font data for 1 line
	LSL	tmp3, XP
	ORR	tmp2, tmp3		; write with OR
	STR	tmp2, [BPaddr1]
	ADD	fptr, #1
	ADD	BPaddr1, #4
	SUB	i1, #1
	BNE	%B01

	ADD	XP, #8			; move X coordinate
	MOV	tmp2, #1
	ADD	U_CHaddr, tmp2		; next character
	B	%B04
03
	POP	{ r3-r4 }
	MOV	r8, r3
	MOV	r9, r4

	POP	{ r3 }
	MOV	lr, r3
	POP	{ r3-r7 }
	BX	lr

;-----------------------------------------------------------------
	END
