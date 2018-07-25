$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

simple tutorials for using libds.lib

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.



tuto0 - tuto11:
===============

<Description>

	These are the rewritten libcd tutorial for libds.lib.
	Since the structure of the program has been kept as in tact as
	possible, these are not necessarily the most appropriate methods
	for libds.lib, but by comparing these with the tutorial of libcd,
	you should be able to grasp the characteristics of libds.

tuto12:
=======

<Description>

	The sample for the comparison of the time required until the
	playing of the music for the cases of using forward seek to play
	DA and not.

	The time required until the playing of "TIME TO START" displayed
	on the screen (frame count: 1/60 sec. units). You can grasp the
	amount of time required for the seek through this.

<How to Execute>

	> psymake tuto12
	> run tuto12

<Controller Button Assignment>

	Directional Button	:	Select track.
				        (when the direction is being sought,
					the seek starts at the beginning of
					the music).
	Triangle Button         :	Forward seek ON/OFF switch.
	Cross Button            :	Play start.
	Select Button           :	Terminate program.

<Remarks>

	Since the actual time required for the seek depends on the
	conditions (head position, file location and other environmental
	issues), the data obtained by this sample should only be used as
	a guide to get rough idea.

tuto13:
=======

<Description>

	The sample for the comparison of the time required until the
	playing of the music for the cases of using forward seek to play XA
	and not.

	The time required until the playing of "TIME TO START" displayed on
	the screen (frame count: 1/60 sec. units). You can grasp the amount
	of time required for the seek through this.

<How to Execute>

	> psymake tuto13
	> run tuto13

<Controller Button Assignment>

	Directional Button	:	XA channel selection.
				        (when the direction is being sought,
					the seek starts at the beginning of
					the music).
	Triangle Button         :	Forward seek ON/OFF switch.
	Square Button           :	Forward seek ON _F Seek start at the
					beginning of the file.
					Forward seek OFF _F Seek start at the
					beginning of the CD.
	Cross Button            :	Play start.
	Select Button           :	Terminate program.

<Remarks>

	Since the actual time required for the seek depends on the
	conditions (head position, file location and other environmental
	issues), the data obtained by this sample should only be used as
	a guide to get rough idea.

tuto14:
=======

<Description>

	The sample of comparison of the time required for continuous
	reading of multiple files (sequential recording on the CD and
	reading without seeking individual file at a time, but rather,
	continuously read in recorded sequence) and seeking and reading
	individual files at a time.
	In the sample, the data sector is sectioned in 50 sector units, and
	the access is executed, with the set of 50 sectors regarded as a
	single file.

	"TIME TO READ" displayed on the screen is the time required to read
	50 sectors*10 (frame count: 1/60 sec. units).

<How to Execute>

	> psymake tuto14
	> run tuto14

<Controller Button Assignment>

	Triangle Button         :	Continuous read
					/ Individual access read switch.
	Cross Button            :	Read start.
	Select Button           :	Terminate program.

<Remarks>

	Since the actual time required for the seek depends on the
	conditions (head position, file location and other environmental
	issues), the data obtained by this sample should only be used as
	a guide to get rough idea.
