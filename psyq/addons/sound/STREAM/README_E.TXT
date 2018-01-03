$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

Sample program for SPU streaming:

	   Copyright (C) 1995,1996 by Sony Computer Entertainment Inc.
						  All Rights Reserved.

$Id: readme_e.txt,v 1.2 1996/03/08 07:07:05 kaol Exp $
----------------------------------------------------------------------

This directory includes the sample code using SPU streaming in libSPU.

----------------------------------------------------------------------
tuto1.c:

Control start/stop SPU streaming (max: 14 streams) with control pad.
Status of each streams are displayed.

o Control Pad Assignment:

	Lup:		ball# increment
	Ldown:		ball# decrement
	Rdown:		start 14 SPU streams
	Rright:		stop 14 SPU streams
      [ Rleft:		stop confirm sound   ]
	Rup:		Quit this sample

	with START:
		Lup:		start SPU stream 0 ch.
		Lright:		start SPU stream 1 ch.
		Ldown:		start SPU stream 2 ch.
		Lleft:		start SPU stream 3 ch.
		Rup:		start SPU stream 4 ch.
		Rright:		start SPU stream 5 ch.
		Rdown:		start SPU stream 6 ch.

	with SELECT:
		Lup:		stop SPU stream 0 ch.
		Lright:		stop SPU stream 1 ch.
		Ldown:		stop SPU stream 2 ch.
		Lleft:		stop SPU stream 3 ch.
		Rup:		stop SPU stream 4 ch.
		Rright:		stop SPU stream 5 ch.
		Rdown:		stop SPU stream 6 ch.

o How to run:

	> psymake
	> psymake load
	> run main

----------------------------------------------------------------------
tuto2.c:

This sample is derived from sample\cd\movie\tuto0.c, mixing CD
streaming and SPU streaming.

Behavior about SPU streaming is the same as tuto1.c.

o Control Pad Assignment:

	Rdown:		start 14 SPU streams
	Rright:		stop 14 SPU streams
      [ Rleft:		stop confirm sound   ]
	Rup:		Rewind animation data

	with START:
		Lup:		start SPU stream 0 ch.
		Lright:		start SPU stream 1 ch.
		Ldown:		start SPU stream 2 ch.
		Lleft:		start SPU stream 3 ch.
		Rup:		start SPU stream 4 ch.
		Rright:		start SPU stream 5 ch.
		Rdown:		start SPU stream 6 ch.

	with SELECT:
		Lup:		stop SPU stream 0 ch.
		Lright:		stop SPU stream 1 ch.
		Ldown:		stop SPU stream 2 ch.
		Lleft:		stop SPU stream 3 ch.
		Rup:		stop SPU stream 4 ch.
		Rright:		stop SPU stream 5 ch.
		Rdown:		stop SPU stream 6 ch.

	START and SELECT:
		Quit this sample

o How to run:

	> psymake
	> psymake load
	> run main

----------------------------------------------------------------------
	   Copyright (C) 1995,1996 by Sony Computer Entertainment Inc.
