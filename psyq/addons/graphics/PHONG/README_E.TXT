$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

              	PHONG SHADING SAMPLE PROGRAM


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


<How to operate>
	PADRup/down:	Rotation about Z-axis ( rotations about two centers）
	PADRleft/right:	Rotation about Y-axis ( rotations about two centers）
	PADLup/down:	Rotation about Z-axis ( rotations about one common center）
	PADLleft/right:	Rotation about Z-axis ( rotations about one common center）
	PADselect:	Terminate

