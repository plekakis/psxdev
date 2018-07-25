
This directory contains an example of a (very simple) sub program running
under none2.lib and returning control to bs.exe. There is a cti file so
that you can make an emulator image, and run this stuff from the emulator.
The program harness.c is an example of a program that will return control,
and also initialise the various subsystems and close them down properly too;
if you don't shut them down properly, bs.exe will explode. bs.exe has a
timeout, so if something goes wrong in bs it will reboot after about 5 seconds.

Also, harness.c shows how to recover the demo mode, timeout in seconds, and
the index of the CD track that your program will use (if you use DA audio).
This is because I may have to rearrange the CD tracks on the CD, so you
can't guarantee what was track 1 is going to be track 1 on the demo CD.

Good luck,

Allan.


