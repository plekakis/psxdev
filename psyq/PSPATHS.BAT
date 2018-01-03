@ECHO OFF
REM ================= PSX DEVELOPMENT ENVIRONMENT VARIABLES =============
REM       RELEASE 1.8 LIBRARY 3.6.1 Date: 1-31-1998 Time: 21:38:44
set PATH=C:\Psyq\bin;%PATH%
set PSX_PATH=C:\Psyq\bin
set LIBRARY_PATH=C:\Psyq\lib
set C_PLUS_INCLUDE_PATH=C:\Psyq\include
set C_INCLUDE_PATH=C:\Psyq\include
set PSYQ_PATH=C:\Psyq\bin
 
 
REM     Does your computer lack a floating-point co-processor?
REM     If so, uncomment the following line:
REM             set GO32=emu d:\ps\bin\emu387
 
REM =====================================================================
REM EXAMPLE DEVICE DRVIERS. Modify according
REM to information in the "2000.doc" or "2500.doc" document
REM in the Technical Reference CD.
REM For the DTL-H2000 (ISA) board: 
REM d:\ps\bin\dexbios /a1340 /i11
REM For the DTL-H2500 (PCI) board (decicons only): 
REM d:\ps\bin\h25drv.exe /v /n
REM For the CD-ROM emulator (ISA) board:
REM d:\ps\bin\cdbios /a388 /i7 /d6 
set COMPILER_PATH=C:\Psyq\bin
set GO32=DPMISTACK 1000000 
set G032TMP=C:\WINDOWS\TEMP
set TMPDIR=C:\WINDOWS\TEMP
 
  

 