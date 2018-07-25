This directory contains the startup object file.

This object file replaces the startup code in libsn.lib. If you have linked
your executable with startup.obj, your program will no longer be able to be
launched as a normal playstation executable, but must instead be launched
from a separate program. 

The startup code in startup.obj differs from the code in libsn.lib in the
following ways: 
	1. startup.obj does not reinitialize the heap. 
	2. startup.obj does a nop when you fall out of main, whereas libsn 
		does a break $1
	3. startup.obj does not set up a new stack pointer.

Like libsn, startup.obj provides the hand variables _ramsize, _stacksize 
(for artifically setting the sizes of these things), and __heapbase, 
__heapsize, etc etc for telling where various segments of your program are.

Have fun launching!
