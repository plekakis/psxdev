$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$	
        +-----------------------------------------------------+
	|///    Sound & Graphics sample program for PC    /// |
	+-----------------------------------------------------+


o SAMPLE:

	This is a sample program using both sound and graphics.

	main.c : play the SEQ data file and beep sounds when 
		 balls striked walls.

o First, do make.

	> psymake

	Then, load waveform data (.vab file) and sound sequence data 
	(.seq file) to SPU. To load such files to SPU, pqbload is used.

	Usage: pqbload file addr

	Sample source required:

	> pqbload sample.vab 80015000      ... load waveform data
        > pqbload mozart.seq 80010000
                        ... load sound sequence data (SEQ data file)


o controller:

	UP button:		Whitsle(high)
	DOWN button:		Whitsle(low)
	RIGHT button:		Handclap
	LEFT button:		Bass drum
	Square button:		Tom(high)
	X button:		Tom(low)
	Triangle button:	Conga(low)
	O button:		Conga(high)
	N button:		Human voice, "GOMENNE"
	L button:		Snae
	SELECT:			Quit program.

	Balls move up and down while buttons is pressed, and
	beep sounds when balls striked walls.

o Launch the program:
        
        > pqbload sample.vab 80015000
        > pqbload mozart.seq 80010000
        > run main

	                Copyright (C) 1994 by Sony Computer Entertainment
