$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
mutual: Read sound data from CD

		      Copyright(C) 1997 Sony Computer Entertainment Inc.
						    All Rights Reserved.

<Descriptions>
  Plays the 2 SEQ files with reading SEQ and VH/VB data from CD (or
  from main memory).  Several situation can be chosen in source code
  level (use #define).

  **** Notice ****
  This sample code is written originally for reading the files below
  from CD:

	\PSX\DATA\VAB\MUTUAL.VH
	\PSX\DATA\VAB\MUTUAL.VB
	\PSX\DATA\SEQ\MUTUAL1.SEQ
	\PSX\DATA\SEQ\MUTUAL2.SEQ

  About the #define option is described in the comment header of
  `main.c'.

<Controller Button Assignment>
	Up    Directional Button:  sound #1: load and transfer
	Right Directional Button:  sound #1: play sound
	Left  Directional Button:  sound #1: stop sound
	Triangle	  Button:  sound #2: load and transfer 
	Circle		  Button:  sound #2: play sound
	Square		  Button:  sound #2: stop sound
	L1		  Button:  decrement # of ball
	R1		  Button:  increment # of ball
	Select		  Button:  Quit

<How to execute>
  1.   > psymake		compile

  2.a. > psymake load		data transfer
       (#define START_PLAY or #undef WITH_CDROM)
		or
  2.b. Insert Data disc (include the SEQ/VH/VB files above) in
       DTL-H2010 (or DTL-H2510)
       (#define WITH_CDROM)

  3.   > run main		run
