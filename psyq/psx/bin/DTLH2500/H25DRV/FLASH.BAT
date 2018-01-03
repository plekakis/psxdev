@echo off
rem
rem    flash.bat  Version 1.0
rem    Copyright (C) 1996 by Sony Computer Entertainment Inc.
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
  freset -u%UNIT% -r 1
    @if errorlevel 1 goto errfin

  fload -u%UNIT% %IMG%
    @if errorlevel 1 goto errfin
  @echo wait...
  fquit -u%UNIT% -v0
    @if errorlevel 1 goto errfin
  freset -u%UNIT% -v0 1
    @if errorlevel 1 goto errfin
goto allfin

:errfin
  @echo ERROR !
:allfin
set IMG=
set UNIT=
