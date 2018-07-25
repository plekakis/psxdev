@echo off
rem
rem    flash.bat  Version 2.0
rem    Copyright (C) 1996 by Sony Computer Entertainment Inc.
rem			$Date: 1996/12/25 12:05:12 $
rem
rem  Usage:
rem	flash.bat
rem	flash.bat Image_file
rem     flash.bat Image_file unit_number
rem

set IMG=h2500.img
set UNIT=0
@if "%1" == "" goto stdimg
  set IMG=%1
:stdimg
@if "%2" == "" goto stdunit
  set UNIT=%2
:stdunit
  flash25 -u%UNIT% %IMG%
    @if errorlevel 1 goto errfin

rem rem OLD_TOOL
rem  freset -u%UNIT% -r 1
rem    @if errorlevel 1 goto errfin
rem
rem  fload -u%UNIT% %IMG%
rem    @if errorlevel 1 goto errfin
rem  @echo wait...
rem  fquit -u%UNIT% -v0
rem    @if errorlevel 1 goto errfin
rem  freset -u%UNIT% -v0 1
rem    @if errorlevel 1 goto errfin
goto allfin

:errfin
  @echo ERROR !
:allfin
set IMG=
set UNIT=
