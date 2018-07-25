$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

----------------------------------------------------------------
[Read sound data from CD]

Plays the 2 SEQ files with reading SEQ and VH/VB data from CD (or from
main memory).  Several situation can be chosen in source code level
(use #define).

================================================================
[!!!! Notice !!!!]
This sample code is written originally for reading the files below
from CD:

	\DATA\MUTUAL.VH
	\DATA\MUTUAL.VB
	\DATA\MUTUAL1.SEQ
	\DATA\MUTUAL2.SEQ

================================================================

About the #define option is described in the comment header of
`main.c'.

o Control Pad Assignment:

		L2			R2
		L1 ball dec.		R1 ball inc.

		 ^ load		       [Tri] load
         stop <-   -> play   stop [Squ]     [Cir] play

	      sound 1:                sound 2:

			[select]: Quit

o How to run:

	> psymake
	> psymake load
	  (#define START_PLAY or #undef WITH_CDROM)
		or
	Insert Data disc (include the SEQ/VH/VB files above) in
	DTL-H2010
	  (#define WITH_CDROM)

	> dbugpsx main /r50 /e

	                Copyright (C) 1994 by Sony Computer Entertainment Inc.
