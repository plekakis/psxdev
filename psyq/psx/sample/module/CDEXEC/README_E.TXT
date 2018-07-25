$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

module\cdexec : DTL-H2000 Boot Module

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


<Description>

  A module for executing PSX.EXE on a CD-ROM/CD-Emulator with DTL-H2000 
  (A way to boot with patchx executed.)

  At present, in order to load a program (.cpe file) from PC to carry 
  out, patchx.cpe must be carried out in advance.

	Example:
	DOS> run patchx  
	DOS> run main
  
  On the other hand, in order to read a boot program (PSX.EXE) 
  on the CD-ROM/CD-Emulator to carry out, cdexec.cpe must be carried 
  out after patchx.cpe execution.

	DOS> run patchx
	DOS> run cdexec


<How to Execute>

	> psymake
	> run patchx
	> run cdexec


<Remarks>

We can use 'resetps 0' to reboot the target, but RESETPS invalidate
the patch on DTL-H2000.
'resetps 0' don't run correctly on H2000.
CDEXEC was packed as sample to escape from this problem.

'resetps 0' run correctly on H2500,H2700 and you need not use CDEXEC.



