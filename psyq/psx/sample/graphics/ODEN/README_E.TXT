$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

		PSX Lighting Demo

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>


This sample is the demo for realtime lighting calculation using libgs.

You can set the color and brightness of the lights and move the direction
of the lights by the control pad.

The models are ball , cone and cube with texture on/off.

Cube model is subdivided by libgs divide function to reduce distortion
of texture.

<How to Execute>

	> psymake
	> psymake load
	> run oden

<Controller Button Assignment>

<when not push upper keys>

right keys : rotation of objects (X axis and Y axis)
left arrow keys up right left : texture mapping of each objects
left arrow key down : unmap texture of three objects
select : farer objects
start  : nearer objects
select & stop : exit program

<when push upper buttons except upper right near button>

        Up Directional Button   :	cntral object texture on
        Right Directional Button:	right object texture on
        Left Directional Button :	left object texture on
        Square Button           :	rotations objects
        Cross Button            :	rotations objects
        Triangle Button         :	rotations objects
        Circle Button           :	rotations objects
        L1 Button               :	not pushed
        L2 Button               :	not pushed
        R1 Button               :	not pushed
        R2 Button               :	not pushed
        Start Button            :	nearer the objects
        Select Button           :	farer the objects
	Start&Select Button	:	Exit The Program

<when push upper right near button>

        Up Directional Button   :	change the direction of ligth source
        Down Directional Button :	change the direction of ligth source
        Right Directional Button:	change the direction of ligth source
        Left Directional Button :	change the direction of ligth source
        Square Button           :	decrease light source green
        Cross Button            :	decrease light source red
        Triangle Button         :	increase light source red
        Circle Button           :	increase light source green 
        L1 Button               :	litht source 2
        L2 Button               :	light source 1
        R1 Button               :	light source 3
        Start Button            :	increase light source blue
        Select Button           :	increase light source blue

right up and down keys : control ambient of the light

        Triangle Button         : increase the anbient of the light source
        Circle Button           : decrease the anbient of the light source
        R2 Button               :	PUSHED


<Remarks>

[models]

    --------------------------------------------------------
    | modeling    |  shading              | divide         |
    |------------------------------------------------------|
    |------------------------------------------------------|
    | cube        |  flat shading	  | ON(16 divide)  |
    |------------------------------------------------------|
    | cone        |  gour shading         | OFF            |
    |------------------------------------------------------|
    | ball        |  gour shading         | OFF            |
    --------------------------------------------------------

