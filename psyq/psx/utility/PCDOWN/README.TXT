	Programs to download from PC to PS

Original: 1997/07/14
Modified: July 24, 1997
Modified: February 6, 1998


\pcdown\psx\psx.exe		: program for PlayStation to receive the target files
			  	  and to run them
\pcdown\pcbload\pcbload.exe	: program for PC to send the data file
\pcdown\pcrun\pcrun.exe		: program for PC to send the EXE file

* Operation method

1. Burn psx.exe into CD-R for the bootable PlayStation disc.
2. Connect Serial Port(COM1) of PC to the serial connection port 
  of DebuggingStation with DTL-H3050.
3. Boot DebuggingStation with CD-R in which psx.exe is included.
  Loading correct, you can see black picture on TV.

  Note that the indication on TV will be stopped during loading to gain
  the transmission rate.

4.With DOS prompt mode on Windows95
  (a) Download data file;
      DOS> pcbload <file name> <loading address>
     example : DOS> pcbload sample.tim 80010000
  (b) Download and run EXE file(PS);
      DOS> pcrun <file name>
     example : DOS> pcrun sample.exe

   Transmitting correctly, you can see the message "complete!" on PC
  and run over again (a) to download some data files. When downloading
  executive file, push start button of PSX controller, then executive
  file runs.

   Note when the executive program reads out some data files from 
  CD-R, change your target disc(CD-R) before you push start button.
  And executive file is just EXE file. CPE file is not abailable.

* Transmitting rate

 Although the transmitting rate of RS-232C is set up 57600bps/8bit
/non parity/stop bit 1, the rate is actually about 5kbps. So that it will
take about 20 seconds to complete when downloading the data file of which
the size is 100kbyte. Note that the downloading time takes too much over
your expectation, the transmission may be failed.
Push any key(Exp. Enter KEY) to stop downloading, and reboot DebuggingStation.
During downloading, these programs takes checksum with 128byte/packet.

* libsio.lib

 Use libsio.lib that is over Ver.4.0 to compile the PSX program.

-------------------------------------------------------------------
July 21, 1997. Notes:
 
-The program in "\pcdown\psx" has been compiled and tested.  Make
 sure you run "cpe2x /cA" (or whatever's appropriate) on your "psx.cpe" file.
-Be sure to include your "licenseA.dat" or "licenseE.dat" file for
 burning the PlayStation bootable CD.


February 6, 1998:
- A function call to atoh has been replaced with strtoul().

-------------------------------------------------------------------
Copyright (c) 1998.  Sony Computer Entertainment America Inc.
