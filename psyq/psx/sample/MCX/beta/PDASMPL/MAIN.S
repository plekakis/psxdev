;
; main.S	main user routine
;

PS_MEMCARD	EQU	0	; "Plug into PS and turn memory card features ON"
				; As footnote, "exit when plugged into PS"

	GET	PERI.INC

	IMPORT	MenuSnd
	IMPORT	LCD_Init
	IMPORT	LCD_Clear
	IMPORT	LCD_print
	IMPORT	KEY_Init
	IMPORT	KEY_Read
	IMPORT	KEY_AbortCheck
	IMPORT	TIMER_Init
	IMPORT	TIMER_Start
	IMPORT	TIMER_Stop
	IMPORT	TIMER_Wait
	EXPORT	main
	EXPORT	DAC_Dataset

;=================================================================
; variable (placed in RAM)
;=================================================================
        AREA	MAIN_BSS, DATA, READWRITE
sound_data_offset	%	4	; in RAM so specify different AREA
sound_volume		%	1
;-----------------------------------------------------------------

        CODE32
        AREA	MAIN, CODE, READONLY
;=================================================================
; main routine
;=================================================================
;-----------------------------------------------------------------
; function main
; CODE32
; move "$" on screen and play sound
;
; Usage	UP		play sound
;		DOWN		stop sound
;		RIGHT		enter stand-by mode
;		ENTER		come out of stand-by mode
;		ENTER for 3 seconds	go to exit menu
;
; parameters: none
; Return value: none
; registers used: r0-5
;-----------------------------------------------------------------
XPoint	RN	r5			; register symbol definitions

main
	; initialization settings on start-up
	BL	connect_check		; determine if connected to PS
	SWI	19			; get user interface status
	LDR	r1, [r0]		; r1 interface status contents
	MOV	r0, #0x03
	AND	r0, r0, r1, LSR #18	; 18-19 bit get volume (0: high 1: low 2: off)
	LDR	r1, =sound_volume
	STRB	r0, [r1]		; store in sound_volume

	MOV	r0, #0			; timer number for frame
	LDR	r1, =0x3d09		; 1/32 seconds for 1MHz, CLKDIV=0
	MOV	r2, #0			; CLKDIV=0 (pre-scale value 2)
	BL	TIMER_Init		; initialize timer
	MOV	r0, #2			; timer number for sound
	MOV	r1, #250		; 1 KHz square wave (interrupt is 2KHz)
	MOV	r2, #0			; CLKDIV=0 (pre-scale value 2)
	BL	TIMER_Init		; initialize timer
	MOV	r0, #0
	BL	TIMER_Start		; start frame timer
	MOV	XPoint, #0		; set initial X coordinate value

main_init
	; initial settings when restarted
	MOV	r0, #0
	MOV	r1, #32*3
	BL	KEY_Init		; initialize key entry
	MOV	r0, #2			; frame rate 32 Hz
	MOV	r1, #0			; display previous frame
	BL	LCD_Init		; initialize LCD

main_loop
	; monitor various statuses
	SWI	6			; get PDA status
	LDR	r1, [r0]
	TST	r1, #0x800
	BNE	END_fromPS		; exit if exit request received from PS
	TST	r1, #0x100
	BLNE	switch_mode		; switch mode if card is plugged in or removed

	MOV	r1, #INTStatus
	LDR	r0, [r1, #INTRawStatus_ofs]
	TST	r0, #0x0400
	BLNE	battery_low		; battery consumption operations

	; display screen
	MOV	r0, #0x00
	BL	LCD_Clear		; clear LCD
	MOV	r0, XPoint
	MOV	r1, #12
	ADR	r2, print_data
	BL	LCD_print		; display characters
	CMP	XPoint, #32		; set X coordinate for next frame
	ADDNE	XPoint, XPoint, #1
	MOVEQ	XPoint, #0

	; wait for frame timer
	MOV	r0, #0
	BL	TIMER_Wait		; wait 1/32 s for timer interrupt

	; key entry operation
	BL	KEY_AbortCheck		; check to see if ENTER pressed for 3 seconds
	CMP	r0, #0			; to 01 if return value is 0
	BEQ	%F01
	BL	Cont_Exit		; to exit menu screen
	CMP	r0, #1			; if return value is 1
	BEQ	END_myself		; exit voluntarily
	BNE	main_init		; continue if 0 (restart)
01
	BL	KEY_Read		; button entry is returned as return value in r0
	MOV	r4, r0

	TST	r4, #BUTTON_UP		; DAC-ON (turning this ON immediately before playing
	BLNE	DAC_ON			; is desirable in terms of battery consumption)
	TST	r4, #BUTTON_UP
	MOVNE	r0, #2
	BLNE	TIMER_Start		; start TC2 timer

	TST	r4, #BUTTON_DOWN
	MOVNE	r0, #2
	BLNE	TIMER_Stop		; stop TC2 timer
	TST	r4, #BUTTON_DOWN	; DAC-OFF (should be turned OFF 
	BLNE	DAC_OFF			; right after playing)

	TST	r4, #BUTTON_RIGHT
	MOVNE	r0, #0
	BNE	standby			; enter stand-by mode

	B	main_loop

;-----------------------------------------------------------------
	; battery consumption operations
	; what happens here is up to the application. In this sample,
	; the application exits and returns to the start-up application.
	; To display warning in application, the use of the
        ; "battery warning mark" in the system font is recommended.
battery_low
	B	END_myself

;-----------------------------------------------------------------
	; general kernel mode switching operation
	;
        ; when plugged in to PS, the kernel inverts the LCD in the BATIRQ operations.
	; when unplugged, the kernel automatically restores the LCD.
	;
	; SWI 5: looks at the PDA status, and if an insertion/removal flag is set
	;         the PDA status contents are updated. Also, operations similar to those
	;         performed for [SWI 17 parameter 1] are performed. If there is no insertion/removal, nothing is done.
	; SWI 17 parameter 1: if plugged in to the PS, enable communication to the PS at 4 MHz
	;                if not plugged in, disable communication with the PS
	; SWI 17 parameter 0: disable communication with the PS
switch_mode
 IF :DEF: PS_MEMCARD		; conditional assembly
	STMDB	sp!, {r0-r1, lr}
	SWI	5		; if plugged in, enable PS communication (4 MHz)
				; if not plugged in, disable PS communication
	SWI	6		; reexamine status
	LDR	r1, [r0]
	TST	r1, #0x200
	MOVEQ	r0, #5		; if PS communication is not possible (i.e., if "removed")
	SWIEQ	4		; then restore clock to 1 MHz
	LDMIA	sp!, {r0-r1, pc}
 ELSE
	B	END_myself
 ENDIF

;-----------------------------------------------------------------
	; determination of connection with PS at startup
	; in some cases, startup will be performed from the PS side. what to do
	; in such cases must be decided. in this sample, the same status used for
	; card insertion is used.
connect_check
	STMDB	sp!, {r0-r1, lr}
	MOV	r1, #PIOControl
	LDR	r0, [r1, #PIOReadInputData_ofs]
	TST	r0, #0x10		; if connected to PS
	MOVNE	r1, #LCDControl
	LDRNE	r0, [r1]
	ORRNE	r0, r0, #0x80
	STRNE	r0, [r1]		; invert LCD
	MOVNE	r0, #1
	SWINE	17			; enable communication with PS
	LDMIA	sp!, {r0-r1, pc}

;-----------------------------------------------------------------
	; standard stand-by operations
	; in this sample, stand-by mode is entered by pressing the right button
	; but the general specification is "enter standby automatically if no key entry for a fixed interval"
standby
	MOV	r1, #PIOControl
	MOV	r0, #0x02		; PIO1 to output
	STR	r0, [r1]		; (if input, the current consumption is greater)
	MOV	r0, #0x62
	STR	r0, [r1, #PIOSetOutputData_ofs]	; LED OFF, speaker standby
					; IR at high-output mode (this provides lower consumption)
	MOV	r0, #0x00
	STR	r0, [r1, #DACControl_ofs]	; DAC standby
 	MOV	r0, #0x07		; band gap, voltage comparator STANDBY
	STR	r0, [r1, #LVDControl_ofs]	; periodic voltage test mode ON

	MOV	r1, #IFStaticControl
	MOV	r0, #0x02
	STR	r0, [r1]		; IF standby

	MOV	r1, #LCDControl
	LDR	r0, [r1]
	BIC	r0, r0, #0x48
	STR	r0, [r1]		; charge pump OFF, display OFF

	MOV	r0, #INTStatus
	MOV	r1, #0x0200		; if RTC stays enabled, disable since otherwise there would be
	STR	r1, [r0, #INTEnableReset_ofs]	; a wakeup every second

	MOV	r1, #PMFrequency	; to stand-by mode
	MOV	r0, #0x01
	STR	r0, [r1, #PMStandby_ofs] ; stop clock until wakeup

		; CPU is sleeping during this time
		; wakeup can take place as a result of one of the four following factors: EXT0, RTC, VOL, BATIRQ
		; (interrupts must be enables)
		; in this sample, EXT0, BATIRQ result in wakeups

	MOV	r0, #INTStatus
	MOV	r1, #0x0200
	STR	r1, [r0, #INTEnableSet_ofs]	; enable RTC

	B	main_init		; must reinitialize LCD, etc.
					; so return to main_init

;-----------------------------------------------------------------
	; exit voluntarily
	; in this sample, restore own icon for the "startup application"
END_myself
	MOV	r0, #INTStatus		; operations started after button is released
	LDR	r1, [r0, #INTRawStatus_ofs]	; so the next application is not affected
	TST	r1, #0x3f
	BNE	END_myself

	BL	END_process	; preparations for exiting
	SWI	22		; get own block number
	MOV	r2, r0
	MOV	r0, #1
	MOV	r1, #0		; next, run the "startup application"
	ORR	r2, r2, #0x30	; go to own icon screen
	SWI	8
	MOV	r0, #0		; Cold Start next time
	SWI	9		; end

;-----------------------------------------------------------------
	; exit in response to exit request from PS
	; the application number and parameters for the application to be run next have already
        ; been notified to the kernel via McxExecApr() from the PS side, so these do not need to be set here.
END_fromPS
	BL	END_process	; preparations for exiting
	MOV	r0, #0		; Cold Start next time
	SWI	9		; end

;-----------------------------------------------------------------
	; standard preparation for exiting of application
END_process
	STMDB	sp!, {r0-r1, lr}
	MOV	r0, #0
	SWI	17			; disable communication with the PS

	MOV	r1, #PIOControl
	MOV	r0, #0x02		; PIO1 to output
	STR	r0, [r1]		; (if input, the current consumption is greater)
	MOV	r0, #0x62
	STR	r0, [r1, #PIOSetOutputData_ofs]	; LED OFF, speaker standby
					; IR at high-output mode (this provides lower consumption)
	MOV	r0, #0x00
	STR	r0, [r1, #DACControl_ofs]	; DAC standby

	MOV	r1, #IFStaticControl
	MOV	r0, #0x02
	STR	r0, [r1]		; IF standby

	MOV	r0, #0			; stop all timers
	BL	TIMER_Stop
	MOV	r0, #1
	BL	TIMER_Stop
	MOV	r0, #2
	BL	TIMER_Stop

	MOV	r0, #INTStatus		; set r0 to INTStatus address
	LDR	r1, =0x3fff
	STR	r1, [r0, #INTEnableReset_ofs]	; disable all IRQ, FIQ

	MOV	r0, #1		; delete user IRQ, FIQ callback registrations
	MOV	r1, #0
	SWI	1
	MOV	r0, #2
	MOV	r1, #0
	SWI	1

	MOV	r0, #INTStatus
	MOV	r1, #0x0a00
	STR	r1, [r0, #INTEnableSet_ofs]	; enable BAT, RTC
	LDMIA	sp!, {r0-r1, pc}

;=================================================================
; subroutine
;=================================================================
;-----------------------------------------------------------------
; function DAC_ON
; CODE32
; DAC, speaker ON
; parameters: none
; Return value: none
; registers used: r0-r1
;-----------------------------------------------------------------
DAC_ON
	STMDB	sp!, {r0-r1, lr}
	MOV	r1, #PIOControl
	MOV	r0, #0x20
	STR	r0, [r1, #PIOClearOutputData_ofs]	; speaker ON
	MOV	r0, #0x01
	STR	r0, [r1, #DACControl_ofs]		; DAC active
	LDMIA	sp!, {r0-r1, pc}

;-----------------------------------------------------------------
; function DAC_OFF
; CODE32
; DAC, speaker OFF
; parameters: none
; Return value: none
; registers used: r0-r1
;-----------------------------------------------------------------
DAC_OFF
	STMDB	sp!, {r0-r1, lr}
	MOV	r1, #PIOControl
	MOV	r0, #0x20
	STR	r0, [r1, #PIOSetOutputData_ofs]		; speaker OFF
	MOV	r0, #0x00
	STR	r0, [r1, #DACControl_ofs]		; DAC standby
	LDMIA	sp!, {r0-r1, pc}

;-----------------------------------------------------------------
; function Cont_Exit
; CODE32
; exit menu screen operations determined by TRC
; parameters: none
; return value: r0	0: continue has been selected
;		1: exit has been selected
; registers used: r1-r5,r8
;-----------------------------------------------------------------
Cont_Exit
	STMDB	sp!, {r1-r5, r8, lr}
	MOV	r0, #INTStatus
	LDR	r5, [r0, #INTEnableSet_ofs]	; save current status
	LDR	r1, =0x35c0			; disable IRQ, FIQ other than 
	STR	r1, [r0, #INTEnableReset_ofs]	; BAT,RTC,EXT0-5

	BL	Menu_Sound		; play MenuSnd

	MOV	r0, #0x00
	BL	LCD_Clear		; clear LCD
	MOV	r0, #0
	MOV	r1, #0
	BL	KEY_Init		; initialize key entry
	MOV	r8, #0			; default is r8=0 (Continue)
loop1
	SWI	6			; get PDA status
	LDR	r1, [r0]
	TST	r1, #0x100
	BNE	%F10			; if plugged in, leave

	LDR	r2, =BackPlane0
	ADD	r2, r2, #4*8
	ADR	r0, Cont_data
	MOV	r3, #7
01
	LDR	r1, [r0], #4
	CMP	r8, #0
	MVNEQ	r1, r1			; if r8=0, invert display
	STR	r1, [r2], #4
	SUBS	r3, r3, #1
	BNE	%B01

	LDR	r2, =BackPlane0
	ADD	r2, r2, #4*17
	ADR	r0, Exit_data
	MOV	r3, #7
02
	LDR	r1, [r0], #4
	CMP	r8, #1
	MVNEQ	r1, r1			; if r8=1, invert display
	STR	r1, [r2], #4
	SUBS	r3, r3, #1
	BNE	%B02

	BL	KEY_Read		; button entry is returned as return value in r0
	MOV	r4, r0

	TST	r4, #BUTTON_ENTER
	BEQ	%F03
	BL	Menu_Sound		; play MenuSnd
	CMP	r8, #0
	MOVEQ	r0, #0			; if r8=0, assume CONTINUE return value 0
	MOVNE	r0, #1			; if r8=1, assume EXIT return value 1
	B	%F10			; return
03
	TST	r4, #BUTTON_UP | BUTTON_DOWN
	EORNE	r8, r8, #1		; change display
	BLNE	Menu_Sound		; play MenuSnd
	B	loop1
10
	MOV	r1, #INTStatus
	STR	r5, [r1, #INTEnableSet_ofs]	; restore status
	LDMIA	sp!, {r1-r5, r8, pc}

;-----------------------------------------------------------------
; function Menu_Sound
; CODE32
; play MenuSnd (Menusnd.o is distributed via SCE-NET, etc.)
; parameters: none
; Return value: none
; registers used: r0-r2
;-----------------------------------------------------------------
Menu_Sound
	STMDB	sp!, {r0-r2, lr}
	BL	DAC_ON			; DAC ON
	LDR	r1, =sound_volume
	LDRB	r0, [r1]		; r0 volume
	CMP	r0, #2			; if volume is [2: off]	
	MOVEQ	r0, #0x10		; parameter to 0x10
	BL	MenuSnd
	BL	DAC_OFF			; DAC OFF
	LDMIA	sp!, {r0-r2, pc}

;-----------------------------------------------------------------
; function DAC_dataset
; CODE32
; set data to DACData (called from FIQ_Callback_TC2)
; parameters: none
; Return value: none
; registers used: r0-r4
;-----------------------------------------------------------------
DAC_Dataset
	STMDB	sp!, {r0-r4, lr}
	LDR	r1, =sound_data			; r1 sound_data starting address
	LDR	r0, =sound_data_offset		; r0 offset save address
	LDR	r4, [r0]			; r4 offset
	ADD	r2, r4, r1			; r2 data save address
	LDRH	r3, [r2]			; r3 data
	CMP	r3, #0x400			; if reached end
	LDREQH	r3, sound_data			; put starting data in r3
	MOVEQ	r4, #0				; put offset in 0
	ADD	r4, r4, #2
	STR	r4, [r0]

	LDR	r1, =DACData
	LDR	r0, =sound_volume
	LDRB	r2, [r0]			; r2 volume
	CMP	r2, #2				; if volume is [2: off]	
	MOVEQ	r3, #0				; data to 0
	CMP	r2, #1
	MOVEQ	r3, r3, LSR #2			; if [1: small], then divide data by 4
	MOV	r3, r3, LSL #6			; if [0: large], then leave unchanged
	STR	r3, [r1]			; store in DACData
	LDMIA	sp!, {r0-r4, pc}

;=================================================================
; various data types (data in FlashROM)
;=================================================================
	ALIGN
print_data
	DCB	'$',0
	ALIGN
sound_data
	DCW	0x1ff,0x200,0x17f,0x280,0x0ff,0x300
	DCW	0x07f,0x380,0x0ff,0x300,0x17f,0x280
	DCW	0x400				; exit code
	ALIGN
Cont_data					; CONTINUE image
	DCD	0x00000000,0x7575dddc,0x15549544,0x75549544
	DCD	0x15549544,0x775495dc,0x00000000
Exit_data					; EXIT image
	DCD	0x00000000,0x00755c00,0x00254400,0x00249c00
	DCD	0x00254400,0x00255c00,0x00000000

;-----------------------------------------------------------------
	END

