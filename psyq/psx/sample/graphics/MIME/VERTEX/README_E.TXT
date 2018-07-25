$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

vertex:  sample code for MIMe (vertex-MIMe) using GsDOBJ5

                  Copyright(C) 1994-1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	This is a sample program for manipulating a model using the
	vertex multiple inbetweening method (hereafter refered to as
	vertex-MIMe). See <Remarks> for details.

<How to Execute>
	> psymake all		compile
	> psymake load		load data
	> run main		run

<Controller Button Assignment>
	Direction Buttons: 		transfer XY
	Square,Cross,Triangle,Circle:   rotation
	START,SELECT Button:		transfer Z
	R1:				MIMe motion-1
	R2:				MIMe motion-2
	L1:				MIMe motion-3
	L2:				MIMe motion-4

<Remarks>

*modeling data

	md0.tmd		Base model
	md1.tmd		Deformed model for MIMe motion-1
	md2.tmd		Deformed model for MIMe motion-2
	md3.tmd		Deformed model for MIMe motion-3
	md4.tmd		Deformed model for MIMe motion-4
	md0.vdf		Differential vertex file
	md0.ndf		Differential normal file

	md0.tmd,md0.vdf and md0.ndf are used in sample program.

	Differential files (VDF & NDF) are created using mimefilt as below.

	mimefilt -n md0.tmd md1.tmd md2.tmd md3.tmd md4.tmd

	(see Programer Tool: mimefilt)

* explanations

	MIMe: Multiple Inbetweening Method is an extended inbetweening
	technique applied to real-time animation.

	MIMe is based on simple equation as below.

	     _n          _n
 	P = > Wi * Pi   > Wi = 1.0		: Original equation.
	     ~i=0        ~i=0

	(P means the description parameter set of animation frame.)
			|
			V
 	         _n
 	P = P0 + > Wi * dPi  ( dPi = Pi-P0 )	: MIMe sample program.
	          ~i=1

	 ( P means the vertex or normal set in TMD.)
	 (dP means vdf or ndf.)

	      _n
	(P0 + > Wi * dPi) is calculated in gteMIMefunc, using GTE.
	      ~i=1
