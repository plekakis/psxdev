$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$ 
	+------------------------------------------+
	|///    Sound sample program for PC    /// |
	+------------------------------------------+


o SAMPLE:

	This is a sample program using sound library.

	main.c : play the SEQ data file and display tempo, volume and
		 status on screen.

	jump.c : using jump-table.

o First, do make.

	> psymake

	Then, load waveform data (.vab file) and sound sequence data 
	(.seq file) to SPU. To load such files to SPU, pqbload is used.

	Usage: pqbload file addr

	Sample source required:

	> pqbload ..\balls\sample.vab 80015000      ... load waveform data
	> pqbload ..\balls\mozart.seq 80010000
			... load sound sequence data (SEQ data file)


o controller:

	START:                  Play SEQ data.
	UP button:              Volume up.
	DOWN button:            Volume down.
	RIGHT button:           Crescendo.
	LEFT button:            Decrescendo.
	Square button:          Pause SEQ data.
	X button:               Replay SEQ data.
	N button:               Ritardando.
	L button:               Accelerando.
	SELECT:                 Stop SEQ data.
	SELECT & START:         Quit program.
 
o Launch the program:
	
	> pqbload ..\balls\sample.vab 80015000
	> pqbload ..\balls\mozart.seq 80010000
	> run simple

	> pqbload ..\balls\sample.vab 80015000
	> pqbload ..\balls\mozart.seq 80010000
	> run jump

			Copyright (C) 1994 by Sony Computer Entertainment
