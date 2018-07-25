

PocketStation Sample Program of Exit Menu Ver.1.1 README


This sample program is only to be an example and the operation will not be guaranteed.
It is suggested this sample program is utilized at your own decision after careful
consideration as Sony Computer Entertainment Inc. will take NO responsibility.   



- List of files - 

readme.txt	This file - introductry information
startup.txt	DTL-H4000 Start up manual
PDAtemp.apj	Project template file for PocketStation(PDA)

[PDAsmpl\]
boot.S		Boot sequence
CBACK.S		Interrupt routine
FONT88.S	8x8 font data
KEY.S		Key input functions
LCD.S		LCD display functions
MAIN.S		Main routine
Timer.S		Timer routine
Mhead.S		Memory Card file header
MACRO.INC	Macro definitive file
PERI.INC	Device register definitive file
STRUCT.INC	Structure definitive file
PDA.apj		Project file
Dbgscr.txt	Script file for Debugger
MENUSND.O       Sound module for PDA exit menu 

[PSprog\]
main.c		"PDA applications loading program" for PlayStation(using libmcrd)
makefile.mak	Make file


- Startup procedures of this sample -

* To startup on DTL-H4000

   Please refer to the "DTL-H4000 Startup manual"(startup.txt) .

* To startup on the PocketStation prototype/the PocketStation.

  1）"Boot application" is in operation.

  2）Insert PocketStation into the controller box of DTL-H2000(2500,2700).
      （LCD display will be flipped vertically）

  3）Open PDAsmpl\PDA.apj by ARM Project Manager.  Build [Release]. 

  4）Build the PlayStation writing program by PSprog\.
      > cd psprog
      > psymake

  5）Startup the PlayStation writing program.  Writing sample on the PDA
      （eg. when used with H2500）
      > h25bios
      > mess1
      > psymake run

  6）Startup the sample program from boot application.

- History of changes - 
  Ver. 1.1
* Exit menu has added which enables to proceed to the exit menu by holding down the 
  ENTER key for 3 seconds.
* Alteration has been made so that it makes possible to "exit when inserted into PS"
  with conditional assembler.
* Alteration has been made so that the vertical flip (ROT) flag cannot be set
  with LCD_Init( )functions.

*** END ***

