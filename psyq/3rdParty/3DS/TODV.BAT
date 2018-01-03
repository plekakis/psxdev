@echo off
rem  Todviewer for 3D Studio IPAS program.
rem  This script is called from 3D Studio IPAS program, 
rem  and invokes TOD previewer on DTL-H2000. 
rem  You have to remove DEXBIOS inorder to use this script.
rem   usage:   todv5  tmd_file  tod_file
rem
rem SET TMDADDR=80020000
rem SET TODADDR=80040000
rem SET PSRESET=C:\PSXGRAPH\BIN\PSRESET
rem SET PSBLOAD=C:\PSXGRAPH\BIN\PSBLOAD.EXE
rem SET PSRUN=C:\PSXGRAPH\BIN\PSRUN.EXE
rem SET MAIN=C:\PSXGRAPH\BIN\TODVIEW5.EXE
rem
c:\psxgraph\bin\psreset
c:\psxgraph\bin\psbload -s %1 80020000 > c:\tmp\out
c:\psxgraph\bin\psbload -s %2 80040000 >> c:\tmp\out
c:\psxgraph\bin\psrun   -s c:\psxgraph\bin\todview5.exe  >> c:\tmp\out


