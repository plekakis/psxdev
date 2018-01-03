;
; PERI.S	Peripherals レジスタ及び各種定義
;

;=================================================================
; Interrupt Controller
;=================================================================

INTStatus   	EQU	0x0a000000
INTRawStatus	EQU	0x0a000004
INTEnableSet	EQU	0x0a000008
INTEnableReset	EQU	0x0a00000c
INTSourceClear	EQU	0x0a000010
INTRawStatus_ofs	EQU	0x04
INTEnableSet_ofs	EQU	0x08
INTEnableReset_ofs	EQU	0x0c
INTSourceClear_ofs	EQU	0x10
INTStatus_r20  	EQU	0xa0		; << 20 = 0x0a000000

;=================================================================
; LCD Controller
;=================================================================

LCDControl	EQU	0x0d000000
BackPlane0	EQU	0x0d000100
BackPlane0_ofs	EQU	0x100
LCDControl_r20	EQU	0xD0		; << 20 = 0x0D000000
BackPlane0_r8	EQU	0x01		; <<  8 = 0x0D000100

;=================================================================
; TIMER
;=================================================================

TIMER0Load	EQU	0x0a800000
TIMER0Value	EQU	0x0a800004
TIMER0Control	EQU	0x0a800008
TIMER1Load	EQU	0x0a800010
TIMER1Value	EQU	0x0a800014
TIMER1Control	EQU	0x0a800018
TIMER2Load	EQU	0x0a800020
TIMER2Value	EQU	0x0a800024
TIMER2Control	EQU	0x0a800028
TIMER0Load_r20		EQU	0xa8		; << 20 = 0x0a800000
TIMER0Value_ofs		EQU	0x04
TIMER0Control_ofs	EQU	0x08

;=================================================================
; RTC
;=================================================================

RTCControl	EQU	0x0b800000
RTCInc		EQU	0x0b800004
RTCClock	EQU	0x0b800008
RTCCalendar	EQU	0x0b80000c
RTCControl_r20	EQU	0xb8		; << 20 = 0x0b800000
RTCInc_ofs	EQU	0x04
RTCClock_ofs	EQU	0x08
RTCCalendar_ofs	EQU	0x0c

;=================================================================
; Power Manager
;=================================================================

PMFrequency	EQU	0x0b000000
PMFrequency_r20	EQU	0xb0
PMStandby	EQU	0x0b000004
PMStandby_ofs	EQU	0x04

;=================================================================
; Memory Map
;=================================================================

REGRemap		EQU	0x06000000
FLASHREMAPStatus	EQU	0x06000004
FLASHACTIVEBlocks	EQU	0x06000008
FLASHCodeController	EQU	0x0600000c
FLASHDataController	EQU	0x06000010
REGRemap_r20		EQU	0x60
FLASHCodeController_ofs	EQU	0x0c
FLASHDataController_ofs	EQU	0x10

;=================================================================
; PIO, DAC & LVD
;=================================================================

PIOControl		EQU	0x0d800000
PIOSetOutputData	EQU	0x0d800004
PIOClearOutputData	EQU	0x0d800008
PIOReadInputData	EQU	0x0d80000c
DACControl		EQU	0x0d800010
DACData			EQU	0x0d800014
LVDControl		EQU	0x0d800020
PIOControl_r20		EQU	0xd8
PIOSetOutputData_ofs	EQU	0x04
PIOClearOutputData_ofs	EQU	0x08
PIOReadInputData_ofs	EQU	0x0c
LVDControl_ofs		EQU	0x20
DACControl_ofs		EQU	0x10

;=================================================================
; IF
;=================================================================
IFStaticControl		EQU	0x0c800000
IFDynamicControl	EQU	0x0c800004
IFDynamicControl_ofs	EQU	0x04

;=================================================================
; BUTTON ASSIGN
;=================================================================

BUTTON_ENTER	EQU	0x01
BUTTON_RIGHT	EQU	0x02
BUTTON_LEFT	EQU	0x04
BUTTON_DOWN	EQU	0x08
BUTTON_UP	EQU	0x10
BUTTON_NOUSE	EQU	0x20

;-----------------------------------------------------------------
	END
