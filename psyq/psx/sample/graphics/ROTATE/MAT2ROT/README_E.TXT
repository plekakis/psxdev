$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$


                       Copyright(C) 1994-1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

mat2rot: Get RPY-angles (roll/pitch/yaw angles) from rotation matrix

<Description>

This sample gives an function example "InvRotMatrixZYX()" which is
opposite of RotMatrix().


Left square (square-A) rotates by controller  directly.
Right square (square-B) rotates as follows:


controller => RPY-angles-A => rotation matrix A => Left square(A)'s coordinate

                |
                v

          InvRotMatrixZYX()

                |
                v

          RPY-angles-B => rotation matrix B => Right square(B)'s coordinate


This sample assumes the order of rotation is Z, Y, then X-axis
(as RotMatrix()).

As there are one or more RPY-angles each rotation matrix, angles-B
may not be same as angles-A.

<How to Execute>

	> psymake all		compile
	> psymake load		load data
	> run tuto0		run

<Controller Button Assignment>
	Up/Down Directional Button:	rotate around X-axis
	Left/Right Directional Button:	rotate around Y-axis
	R1,R2:				rotate around Z-axis

<Remarks>
	The term "Euler's Angles" previously used in this sample has been changed 
	to "RPY-angles(roll-pitch-yaw angles)" to make it more accurate.

