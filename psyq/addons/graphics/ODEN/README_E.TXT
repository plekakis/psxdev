$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

		PSX Lighting Demo

		ODEN		Dec,28,1994


[abstract of the program]

This sample is the demo for realtime lighting calculation using libgs.

You can set the color and brightness of the lights and move the direction
of the lights by the control pad.

The models are ball , cone and cube with texture on/off.

Cube model is subdivided by libgs divide function to reduce distortion
of texture.


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


[how to control]

<when not push upper keys>
right keys : rotation of objects (X axis and Y axis)
left arrow keys up right left : texture mapping of each objects
left arrow key down : unmap texture of three objects
select : farer objects
start  : nearer objects
select & stop : exit program

<when push upper buttons except upper right near button>
Upper three buttons are each three lights's controller.
When you push these buttons and 
left arrow keys : control direction of the light
right up and down keys : control red part of the color of the light
right left and right keys : control green part of the color of the light
stop and select keys : control blue part of the color of the light

<when push upper right near button>
right up and down keys : control ambient of the light

