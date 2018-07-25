@echo off

    if     "%1"=="" goto USAGE
    if not "%1"=="" psxcons -p0x%1,0  auto

    goto EXIT

:USAGE
echo Usage: flash [IO address(hex)]
echo:

:EXIT
