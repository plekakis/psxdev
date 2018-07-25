$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
tuto: Sample program for Fundamental Sound Library

		     Copyright(C) 1997 Sony Computer Entertainment Inc.
						   All rights reserved.

<Descriptions>
  This is sample program for Fundamental Sound Library.

tuto1:
======

<Descriptions>
  Simpel key on/off program.  Key on/off with changing pitch of `sin'
  waveform data transfered by DMA.

<How to execute>
  > psymake tuto1	compile
  > run tuto1		run

<Controller Button Assignment>
	Left  Directional Button:  low-pitch sound
	Up    Directional Button:	^
	Right Directional Button:	:
	Down  Directional Button:	:
	Square		  Button:	:
	Triangle	  Button:	:
	Circle		  Button:	v
	Cross		  Button:  high-pitch sound
	Select		  Button:  Quit

tuto2:
======

<Descriptions>
  `Mute' sample program.
   run sequence is shown below.

	1. Key on (automatically)
	2. Mute On by pushing Start Button
	3. Mute Of by pushing Start Button
	4. Quit	   by pushing Start Button

<How to execute>
  > psymake tuto2	compile
  > run tuto2		run

<Controller Button Assignment>
	Start Button:  change the situation 

tuto3:
======

<Descriptions>
  `SPU Interrupt' sample program.

  o First, two waveform data are transferred by DMA.

	sin wave	(for Interrupt check)
	piano wave	(normal key on/off)

  o Interrupt addres is set to half point of piano
    wave.

  o About a half second after, interrupt will be occurred and sin wave
    is keyed on as the confirmation.

<How to execute>
  > psymake tuto3	compile
  > run tuto3		run

<Controller Button Assignment>
	Start  Button:		 key on `piano' sound
	Select Button:		 key off
	Start and Select Button: Quit

tuto4:
======

<Descriptions>
  `Noise sound' sample program.

<How to execute>
  > psymake tuto4	compile
  > run tuto4		run

<Controller Button Assignment>
	Up Directional	 Button:  sin wave / pitch up
	Down Directional Button:  sin wave / pitch down
	Triangle	 Button:  Noise	   / pitch up
	Cross		 Button:  Noise	   / pitch down

tuto5:
======

<Descriptions>
  `Transferring Partly' sample program.

  Transferring partly the one waveform data to different two areas
  (size: 2048 bytes) mutually, and key on when transferring is
  finished.

<How to execute>
  > psymake tuto5	compile
  > run tuto5		run

<Controller Button Assignment>
	Select Button:	Quit

tuto6:
======

<Descriptions>
  `Reverberation' sample program.

  Key on/off piano waveform data with changing reverberation mode.

<How to execute>
  > psymake tuto6	compile
  > run tuto6		run

<Controller Button Assignment>
	Up Directional	 Button:  Reverberation mode up
	Down Directional Button:  Reverberation mode down
	Start		 Button:  key on
	Select		 Button:  key off
	Start and Select Button:  Quit
