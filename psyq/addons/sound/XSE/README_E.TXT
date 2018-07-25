$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$
	 
        +-------------------------------------------------+
	|///    Sound effect sample program for PC    /// |
	+-------------------------------------------------+


o SAMPLE:

	This is a sample program using sound effect.

	main.c : effect to the voice that keyed on.
		 (pitch bend, auto panning or auto volume)

	If you change the TONENO in main.c from 10 to 9, the voice will 
	change to the one with vibrato.

o First, do make.

	> psymake

	Then, load waveform data (.vab file) to SPU. 
	To load such file to SPU, pqbload is used.

	Usage: pqbload file addr

	Sample source required:

	> pqbload simple.vab 80010000      ... load waveform data

o controller:

	START:			Voice key on.
	SELECT:			Voice key off.
	UP button:		Move up the tile.
	DOWN button:		Move down the tile.
	N button:		Move left the tile.
	L button:		Move right the tile.
	SELECT & START:		Quit program.

o Launch the program:
        
        > pqbload simple.vab 80010000
        > run main

	                Copyright (C) 1994 by Sony Computer Entertainment
