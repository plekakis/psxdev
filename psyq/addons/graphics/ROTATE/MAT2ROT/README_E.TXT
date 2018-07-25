$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

-----------------------------------------------------------------------------
							1996.03.28 SCE

Get Euler's angles from rotation matrix

*overview

This sample gives an function example "InvRotMatrixZYX()" which is
opposite of RotMatrix().


Left square (square-A) rotates by control pad directly.
Right square (square-B) rotates as follows:


pad => Euler's angles-A => rotation matrix A => Left square(A)'s coordinate

             |
             v

       InvRotMatrixZYX()

             |
             v

       Euler's angles-B => rotation matrix B => Right square(B)'s coordinate


This sample assumes the order of rotation is Z, Y, then X-axis
(as RotMatrix()).

As there are one or more Euler's angles each rotation matrix, angles-B 
may not be same as angles-A.

* compile

	> psymake all

* execute

	> psymake load
	> run tuto0

* control

	left cross key up/down:		rotate around X-axis
	left cross key left/right:	rotate around Y-axis
	R1, R2:				rotate around Z-axis


			Copyright (C) 1996 Sony Computer Entertainment Inc.
