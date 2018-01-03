
This directory contains setsp.exe, a PC executable for setting the stack
address for a PlayStation .EXE properly. This is so that the demo disk
boot program knows where to put your stack.

The syntax is:

setsp <my exe> <stack address>

eg  setsp dinosaur.exe 0x801ffff0

Sets the stack address in the XF_HDR of the executable dinosaur.exe to 16 bytes
below the top of two megabytes. You don't have to have '0x', the program
assumes hex (no one specifies stack addresses in decimal!). Setsp will also
check to see where you have put the stack and give warnings if the stack is in
a silly place - 0x0, for example.


Also in this directory is my handy dumpexe.exe program for PCs, which dumps
the information from the first sector of a PSX .EXE file, so you can check to
see where executables are going to get put in RAM and what their stack is.
The syntax is:

dumpexe <my exe>

These program requires DOS4GW.EXE, the Rational Systems DOS extender. This is
available widely (all you PC developers with Watcom) or from our BBS.

And that's all !

Allan.
