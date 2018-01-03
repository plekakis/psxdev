$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

MOVIDE SAMPLES
Thease programs are the sample code for playing back MDEC MOVIE data
on the STR format.

tuto0
=====
<Description>
	The minimum set of playing back movie rutine.
		There are following limitations.

		1 The resolutions of movie are multiples of 16.
		2 There is no dynamic resolution changes.


	This program play back movie data on the STR foramt.
	There are two mode for playing back movie,1st mode is 16 bit/pixel
	and 2nd mode is 24bit/pixel. if you define the 'RGB24' which is
	the macro in the program, the mode is set 24 bit/pixel mode.

	If you change the number of frames to play back , you change the
	END_FRAME macro in the program.

	NOTE!: END_FRAME is 2 or 3 less than the correct end point because
		of unexpectedly lost the frame.
	       If you must make sure to play back the last frame , you had
	       better to copy the last frame two or three times and add
	       the last, and you set the END_FRAME two or three frames
	       previous the last frame.


<How to Execute>

	> psymake tuto0
	> run tuto0

<Controller Button Assignment>

        Select Button           : Exit The Program



tuto1
=====
<Description>

	This code have more funcions than tuto0.c
		Extended fuctions are followings.

		- The parameters for playing back are structured.

		- Free resolutions,but in the 24 bit mode the horizontal
		  resolution must be even.


<How to Execute>
	> psymake tuto1
	> run tuto1

<Controller Button Assignment>
        Select Button           : Exit The Program


tuto2
=====
<Description>

	Added the on-memory-streaming function to tuto0.c

	O button pressed
		Stop updating Ring buffer by setting 0 to end_frame of
		StSetStream().
		And playing back the loop movie on the Ring buffer by
		StGetNextS(), ON MEMORY STREAMING.

	X button pressed
		Back to normal play , update the Ring buffer data again.

<How to Execute>
	> psymake tuto2
	> run tuto2

<Controller Button Assignment>
        Cross Button            :	Start On-Memory-Streaming
        Circle Button           :	Exit From On-Memory-Streaming
        Select Button           :	Exit The Program



tuto3
=====
<Description>

	This program have avoiding frame skip funciton in additon to tuto0.c.
	This skipping frame is caused by full of ring buffer.

	Basic idea is followings.

	When the decode speed is severe,access a little backwoard.
	So the transfer rate from CD is reduced and the ring buffer full
	state will be avoided.

	But using access to CD , you can't use this method for the
	movie with XA audio because the noize will be generated.
	And this sample only run StModeStream2 mode.
	
	This sample is profit for decoding non-skipping frame without
	XA audio.

	In the program I use StRingStatus() and StGetBackloc() fuction
	supported ver 3.4 library later.

	When there are few free sectors reported by StRingStatus,
	you access backward location reported by StGetBackloc().
	These codes are followings.

	StRingStatus(&free,&over); /* added ring buffer status */
        if(free<MINIMUM_FREE_SECTORS) {
	    next_frame = StGetBackloc(&backloc); /* get the latest frame
						    loc and id on ring buf*/
	    StSetMask(1,next_frame,0xffffffff); /* masking from cdrom data */
            /* access backward to reduce trasfer rate */
	    strKickCD(&backloc);
	}

	You must call these codes frame by frame.

	You must set RING_SIZE larger than tuto0.c not to stop movie
	while accessing. You should set RING_SIZE - MINIMUS_SECTORS
	to includes 7 or 8 FRAMEs.

	you must MINIMU_SECTORS lager than 1 FRAME sector size because
	frame will be skipped before you access backward CD.

<How to Execute>
	> psymake tuto3
	> run tuto3

<Controller Button Assignment>
        Select Button           :	Exit The Program


