
REM   Your CD Emulator hard-drive should be 
REM   up and running.  Fire up the cdbios with
REM   this command
REM 
REM          cdbios /a398 /i5 /d7
REM
REM   The numbers are those that work on my system,
REM   but you may need to change them based on your
REM   hardware configuration.


REM   Build the CD image on SCSI "2" (where my hard-drive
REM   is attached), in partion "1", and output the
REM  map to "MyMap.map".  

  buildcd -mMyMap.map -s2:1 hello.cti

