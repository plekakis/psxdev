/*************************************************************************
most important files...
XAPLAY.C   : the CD-ROM XA Audio interface routines.
XAPLAY.H   : prototypes for functions in above.
MAIN.C     : test app for routines in XAPLAY.C.
BALLS.C    : Sony's standard BALL routines for their demo apps.
PACK1.XA   : Multi stream XA file for testing.
PACK2.XA   : Multi stream XA file for testing.
PSX.EXE    : a build of the above code converted to PlayStation EXE
XATEST.CTI : source for a BuildCD session.

You will need to burn at least the XA files onto a CD-Emulator or CD-ROM.
Once there you could do a "run main" as long as the dev system is
looking to the right device (emulator or CD drive) for data.
/*************************************************************************

Description:

This demo show use of interleaved XA streams to store LARGE numbers
of dialog lines, amd how to play them back NEARLY instantly.

The example uses 6 files (pack1.xa -> pack6.xa) each of which has 8 streams
interleaved. A total of 48 dialog lines!

Press the left d-pad up button to trigger playback. A random phrase will be
chosen.

ON SCREEN:
- ID is the phrase to be played (1-48)

- File is the name of the XA file. Note: a jump from PACK1.XA to PACK6.XA
would demonstrate the longest seek time.

- Index is the stream within the file (0-7)

A few notes...

The samples are 37.8k MONO xa. Even at that rate I could interleave
another 8 samples in each file for a total of 16 lines per file. This would
double the available phrases with NO increase in seek time etc.

I could double the number of phrases yet again by going with an 18.9k sample
rate... 32 PHRASES PER FILE!

