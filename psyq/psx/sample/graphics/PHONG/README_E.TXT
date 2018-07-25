$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
PHONG SHADING SAMPLE PROGRAM

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


<Description>
This sample program performs the phong shading on PlayStation.
Since the PlayStation dose not have a hardware with the phong shading capability, 
it must be done by software calculations. Thus in comparison with Gouraud 
shading,	it takes ten times more. 
Please use this phong shading method only for the necessary polygons and for the       
polygons for which this phong shading is effective.  Increasing the numbers of 
polygon in Gouraud shading can obtain the same result in high speed.
In this sample, phone shading is applied to the corner planes of the two decahedrons.
At start time, the left one is drawn by phong shading and the right by Gouraud shading.
When the corner area is relatively small, it would switch from phong to Gouraud shading.
The result of the phong shading is drawn to the texture area once, then it will be drawn again on the screen area as a texture polygon.


<How to Execute>
        >psymake   	Compile
        >run main       Run

<Controller Button Assignment>
        Up Directional Button   :Rotation about Z-axis ( rotations about one common center )
        Down Directional Button :Rotation about Z-axis ( rotations about one common center )
        Right Directional Button:Rotation about Y-axis ( rotations about one common center )
        Left Directional Button :Rotation about Y-axis ( rotations about one common center )
        Square Button           :Rotation about Y-axis ( rotations about two centers )
        Cross Button            :Rotation about Z-axis ( rotations about two centers )
        Triangle Button         :Rotation about Z-axis ( rotations about two centers )
        Circle Button           :Rotation about Y-axis ( rotations about two centers )
        Select Button           :Quit


