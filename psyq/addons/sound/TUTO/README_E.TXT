$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

Sample program for Fundamental Sound Library (the old name: SPU
Library)

		Copyright (C) 1995 by Sony Computer Entertainment Inc.
						  All Rights Reserved.

$Id: readme_e.txt,v 1.5 1995/11/15 10:54:23 kaol Exp $
----------------------------------------------------------------

This is sample program for Fundamental Sound Library (the old name:
SPU Library).

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
\sample\sound\lowlevel\tuto1.c

  Simpel key on/off program.  Key on/off with changing pitch of `sin'
  waveform data transfered by DMA.

  o How to make:

	> psymake tuto1

  o controller assign:

	Button C (left arrow):	low-pitch sound
	Button A (up arrow):		:
	Button D (right arrow):		:
	Button B (down arrow):
	Button G (square):
	Button E (triangle):		:
	Button P (circle):		:
	Button F (cross):	high-pitch sound

	Button K (SELECT):	Quit

  o run
	
	After run `patchx.bat', do `run tuto1'

	> patchx
	> run tuto1


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
\sample\sound\lowlevel\tuto2.c

  `Mute' sample program.

     run sequence is shown below.

	1. Key on
	2. Mute On by pushing button H (START) 
	3. Mute Of by pushing button H (START)
	4. Quit    by pushing button H (START)

  o How to make:

	> psymake tuto2

  o run
	
	After run `patchx.bat', do `run tuto2'

	> patchx
	> run tuto2


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

\sample\sound\lowlevel\tuto3.c

  `SPU Interrupt' sample program.

  o First, two waveform data are transferred by DMA.

	sin wave	(for Interrupt check)
	piano wave	(normal key on/off)

  o Interrupt addres is set to half point of piano
    wave.

  o controller assign:

	Button H (START):	Key on

		About a half second after, interrupt will
		be occurred and key on sin wave.

	Button K (SELECT):	Key off

	Button H & K:		Quit

  o How to make:

	> psymake tuto3

  o run
	
	After run `patchx.bat', do `run tuto3'

	> patchx
	> run tuto3


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

\sample\sound\lowlevel\tuto4.c

  `Noise sound' sample program.

	Button A (up arrow):	sin wave / pitch up
	Button B (down arrow):	sin wave / pitch down
	Button E (triangle):	Noise    / pitch up
	Button F (cross):	Noise    / pitch down

  o How to make:

	> psymake tuto4

  o run
	
	After run `patchx.bat', do `run tuto4'

	> patchx
	> run tuto4


::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

\sample\sound\lowlevel\tuto5.c

  `Transferring Partly' sample program.

	Transferring partly the one waveform data to different two
	areas (size: 2048 bytes) mutually, and key on when
	transferring is finished.

  o controller assign:

	Button K (SELECT):	Quit

  o How to make:

	> psymake tuto5

  o run
	
	After run `patchx.bat', do `run tuto5'

	> patchx
	> run tuto5

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

\sample\sound\lowlevel\tuto6.c

  `Reverberation' sample program.

	Key on/off piano waveform data with changing reverberation
	mode.

  o controller assign:

	Button H (START):	piano key on
	Button K (SELECT):	piano key off
	Button H & K:		Quit

	Button A (up arrow):	Reverb mode up
	Button B (down arrow):	Reverb mode down

  o How to make:

	> psymake tuto6

  o run
	
	After run `patchx.bat', do `run tuto6'

	> patchx
	> run tuto6

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
