$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

	ANIM	SAMPLES FOR HMD ANIMAION

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


animview: GENERAL HMD ANIMATION VIEWER
=====

<Description>

This program is the general viewing prgram for HMD ANIMATION

SEQ/COD displays the target SEQUENCE and MATRIX NO which is overwritten
by the target SEQUENCE

You can select the target SEQUENCE , controlls SEQUENCE ID / SEQUENCE SPEED
by the controller.

In this program , we generate the ROOT coordinate which is linked to the
ROOT coordinte in HMD file. The root cordintate in the program is controlled
by the controller , so you can rotate and transfer the object unless the
ROOT coordinte in the HMD file is drived by the animation sequence.


<How to Execute>
	>psymake animview
	>psymake loadH
	>run animview

<Controller Button Assignment>
        Up Directional Button   :	transfer the object(UP)
        Down Directional Button :	transfer the object(DOWN)
        Right Directional Button:	transfer the object(RIGHT)
        Left Directional Button :	transfer the object(LEFT)
        Square Button           :	rotate the object(LEFT)
        Cross Button            :	rotate the object(DOWN)
        Triangle Button         :	rotate the object(UP)
        Circle Button           :	rotate the object(RIGHT)
        L2/R2 Button            :	inc/dec target sequence
        L1/R1 Button            :	translation objects Z axis
        Start&(L1/R1)           :	inc/dec SPEED for all sequences
        Select&(L1/R1)          :	inc/dec SEQUENCE ID for all sequences
        Start&(L2/R2)           :	inc/dec SPEED
        Select&(L2/R2)          :	inc/dec SEQUENCE ID 
        Select&(Triangle, Square, Cross, Circle):
        				set sequence ID 1, 2, 3, 4 for all
        				sequences
        Select&(Up, Left, Down, Right):	set sequence ID 5, 6, 7, 8 for all
        				sequences
	Start&Slect Button	:	Exit The Program

<Remarks>

[DATA]
	all the HMD data
	ex.
	data/hmd/scei/anim/baloon/baloon.hmd (BEZIER curve motion)


tuto0: GENERAL HMD ANIMATION VIEWER
=====

<Description>

This program is the general viewing prgram for HMD ANIMATION

SEQ/COD displays the target SEQUENCE and MATRIX NO which is overwritten
by the target SEQUENCE

You can select the target SEQUENCE , and rotation , translation,
and inc/dec SEQUENCE ID.
Notice that you can't control the parameters drived by SEQUENCES.


<How to Execute>
	>psymake tuto0
	>psymake load0
	>run tuto0

<Controller Button Assignment>
        Up Directional Button   :	transfer the object(UP)
        Down Directional Button :	transfer the object(DOWN)
        Right Directional Button:	transfer the object(RIGHT)
        Left Directional Button :	transfer the object(LEFT)
        Square Button           :	rotate the object(LEFT)
        Cross Button            :	rotate the object(DOWN)
        Triangle Button         :	rotate the object(UP)
        Circle Button           :	rotate the object(RIGHT)
        L1/L2 Button            :	inc/dec target sequence
        R1/R2 Button            :	translation objects Z axis
        Start Button            :	increase sequence ID
        Select Button           :	decrease sequence ID
	Start&Slect Button	:	Exit The Program

<Remarks>

[DATA]
	data/hmd/scei/anim/tri3/tri.hmd  etc...



tuto1: Interactive animation DEMO by chainging SID
=====

<Description>

	The sample program for interactive animation.

	The data contains several sequences and you can select ths
	sequence by changing the SEQUENCE ID.

<How to Execute>
	>psymake tuto1
	>psymake load1
	>run tuto1

<Controller Button Assignment>

        Up Directional Button   :	transfer the object(UP)
        Down Directional Button :	transfer the object(DOWN)
        Right Directional Button:	transfer the object(RIGHT)
        Left Directional Button :	transfer the object(LEFT)
        Square Button           :	rotate the object(LEFT)
        Cross Button            :	rotate the object(DOWN)
        Triangle Button         :	rotate the object(UP)
        Circle Button           :	rotate the object(RIGHT)
        L2 Button               :	moving tail
	R2 Button               :	moving neck
        Start Button            :	garrop
        Select Button           :	stomp
	Start&Slect Button	:	Exit The Program

<Remarks>

[DATA]
	data/hmd/scei/anim/tri3/tri.hmd


tuto2: Realtime Motion Switch DEMO
=====
<Description>

	Demo Program for Realtime Motion Switch
	
	3 key frames (A B C) are defined.

	A	SID 3
	\--------------- C
	 \		|
	  \		|
	   \		|  SID 2
	    \		|
	     \		|
	      \		|
	 SID 1 \	|
		\	|
		 \	|
		  \	|
		   \	|
		    \	|
		     \  |
		      \	|
		       \| B

	3 sequence are defined.
		1.	A-B
		2.	B-C
		3.	C-A

	SID is 0	: loop current sequence
	SID is 1-3	: switch other sequence

	left arrow keys invoke Realtime Motion Switch
	
	
	
	When these keys are pressed,the sequece is changed immediately.
	These swiching are smoothly,becase the current position is
	defined new key frame and the sequence is interporated this
	the current position keyframe and the target keyframe.

	<Control the SEQUENCE>
	Reverse and Speed are controlable.

	<Restriction in Realtime Motion Switch>
	You can't reverse the sequece when you use Realtime Motion Switch
	becase the system dosn't have the dynamic footprint by Realtime
	Motion Switch.


<How to Execute>
	>psymake tuto2
	>psymake load2
	>run tuto2

<Controller Button Assignment>

        Up Directional Button   :	Realtime Motion Switch
        Down Directional Button :	Realtime Motion Switch
        Right Directional Button:	Realtime Motion Switch
        Left Directional Button :	Realtime Motion Switch
        Square Button           :	switch SEQUENCE ID
        Cross Button            :	switch SEQUENCE ID
        Triangle Button         :	switch SEQUENCE ID
        Circle Button           :	switch SEQUENCE ID
        R1 Button               :	slow motion
	R2 Button               :	accelate the motion speed
        L1 Button               :	slow motion
	L2 Button               :	accelate the motion speed
        Start Button            :	reverse playback
	Start&Slect Button	:	Exit The Program

<Remarks>

[DATA]
	data/hmd/scei/anim/cube0/cube0.hmd


tuto3: Viewpoint animation sample
======

<Description>

	This data contains three coordinates for viewpoint.

	VIEW0
	 
	VIEW1
	 
	VIEW2	<- viewpoint

	The program set the VIEW2 MATRIX to the VIEWPOINT by
	setting the super of GsVIEWUNIT which is set by GsSetViewUnit().

	HMD animation controls the VIEW0,VIEW1,VIEW2 COORDINATE.


	for the subjective view, the top coordinate is object's coordinate.
	So the camera automatically trace the object.

	VIEW1 is the coordiante for setting distace the camera and object.
	VIEW0 is the coordiante for rotating the camera around the object.
	VIEW2 is the coordinate for the angle of the camera.

	This sample invoke three sequence for the camera.

	1. rotate the camera around the object.
	2. the distance of the the camera and the object is long and short.
	3. the angle of the camera is up and down.


<How to Execute>
	>psymake tuto3
	>psymake load3
	>run tuto3


<Controller Button Assignment>
        Up Directional Button   :	transfer the object(UP)
        Down Directional Button :	transfer the object(DOWN)
        Right Directional Button:	transfer the object(RIGHT)
        Left Directional Button :	transfer the object(LEFT)
        Square Button           :	rotate the object(LEFT)
        Cross Button            :	rotate the object(DOWN)
        Triangle Button         :	rotate the object(UP)
        Circle Button           :	rotate the object(RIGHT)
        L1/L2 Button            :	inc/dec target sequence
        R1/R2 Button            :	translation objects Z axis
        Start Button            :	increase sequence ID
        Select Button           :	decrease sequence ID
	Start&Slect Button	:	Exit The Program

<Remarks>

[DATA]
	data/hmd/scei/anim/tri4/tri.hmd

[P.S]
	The model data "tri.hmd" is made from the data created by Viewpoint
	DataLabs and distributed by Newtek Incorporated.

	data/hmd/scei/anim/tri3/tri.hmd
	data/hmd/scei/anim/tri4/tri.hmd


