$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
stream: Sample program for SPU streaming:

		     Copyright(C) 1997 Sony Computer Entertainment Inc.
						   All rights reserved.

<Descriptions>

  This directory includes the sample code using SPU streaming in
  libSPU.

tuto1:
======

<Descriptions>
  Control start/stop SPU streaming (max: 14 streams) with controller.
  Status of each streams are displayed.

<How to execute>
  > psymake		compile
  > psymake load	data transfer
  > run tuto1		run

<Controller Button Assignment>
	Up   Directional Button:  increment # of ball
	Down Directional Button:  decrement # of ball
	Cross		 Button:  start 14 SPU streams
	Circle		 Button:  stop	14 SPU streams
	Triangle	 Button:  Quit
      [ Square		 Button:  stop confirm sound   ]
      [			(available when defined USE_CONFIRM) ]

  Start	 + Up	 Directional Button:  start SPU stream 0 ch.
	   Right Directional Button:  start SPU stream 1 ch.
	   Down	 Directional Button:  start SPU stream 2 ch.
	   Left	 Directional Button:  start SPU stream 3 ch.
	   Triangle	     Button:  start SPU stream 4 ch.
	   Circle	     Button:  start SPU stream 5 ch.
	   Cross	     Button:  start SPU stream 6 ch.

  Select + Up	 Directional Button:  stop  SPU stream 0 ch.
	   Right Directional Button:  stop  SPU stream 1 ch.
	   Down	 Directional Button:  stop  SPU stream 2 ch.
	   Left	 Directional Button:  stop  SPU stream 3 ch.
	   Triangle	     Button:  stop  SPU stream 4 ch.
	   Circle	     Button:  stop  SPU stream 5 ch.
	   Cross	     Button:  stop  SPU stream 6 ch.

tuto2:
======

<Descriptions>
  This sample is derived from sample\cd\movie\tuto0.c, mixing CD
  streaming and SPU streaming.

  Behavior about SPU streaming is the same as tuto1.c.

<How to execute>
  > psymake		compile
  > psymake load	data transfer
  > run tuto2		run

<Controller Button Assignment>
	Cross	 Button:  start 14 SPU streams
	Circle	 Button:  stop	14 SPU streams
	Triangle Button:  Rewind animation data
      [ Square	 Button:  stop confirm sound   ]
      [	  (available when defined USE_CONFIRM) ]

  Start	 + Up	 Directional Button:  start SPU stream 0 ch.
	   Right Directional Button:  start SPU stream 1 ch.
	   Down	 Directional Button:  start SPU stream 2 ch.
	   Left	 Directional Button:  start SPU stream 3 ch.
	   Triangle	     Button:  start SPU stream 4 ch.
	   Circle	     Button:  start SPU stream 5 ch.
	   Cross	     Button:  start SPU stream 6 ch.

  Select + Up	 Directional Button:  stop  SPU stream 0 ch.
	   Right Directional Button:  stop  SPU stream 1 ch.
	   Down	 Directional Button:  stop  SPU stream 2 ch.
	   Left	 Directional Button:  stop  SPU stream 3 ch.
	   Triangle	     Button:  stop  SPU stream 4 ch.
	   Circle	     Button:  stop  SPU stream 5 ch.
	   Cross	     Button:  stop  SPU stream 6 ch.

  Start + Select	     Button:  Quit this sample
