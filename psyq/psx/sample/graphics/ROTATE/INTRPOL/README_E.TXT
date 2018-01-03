$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                       Copyright(C) 1994-1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

intrpol: various kinds of interpolating about rotation

<Description>

	This sample shows interpolation of rotation from any angles to
	any angles by 3 ways of interpolating.

	Push "START" to start interpolating.
	Cube-A (left) shows start position, and Cube-B (right) shows
	end position.

	Cube-C1,C2,C3 (center-top, center-middle, center-bottom) show
	interpolation of the way as follows respectively.

	Cube-C1 (center-top): interpolate about Euler's angles
	Cube-C2 (center-middle): interpolate about rotation matrix elements
	Cube-C3 (center-bottom): interpolate around the rotation axis

	Loci would be shown after interpolation.

	This sample has some preset rotation sets of Cube-A and
	Cube-B. Push "SELECT" to select next sets.
	It's also possible to rotate freely by keys.

<How to Execute>
	> psymake all		compile
	> run main		run

<Controller Button Assignment>
	Directional Buttons,L1/L2 Button          : rotate Cube-A
	Square,Cross,Triangle,Circle,R1,R2 Button : rotate Cube-B
	START:  start interpolating
	SELECT: select preset sets

<Remarks>
* how to interpolate in this sample

  - common function
	interpolation function "func_intrpol()" is as follows:

	func_intrpol(a, b, p) = ((1-p) * a) + (p * b)

	when a is start value, b is end value, and p is parameter of
	interpolation.

  - interpolate about Euler's angles (Cube-C1)

	When Euler's angles of Cube-A is VA(={vax,vay,vaz}) and 
	Euler's angles of Cube-B is VB(={vbx,vby,vbz}), Euler's angles of
	Cube-C1 VC(={vcx,vcy,vcz}) would be:

		vcx = func_intrpol(vax, vbx, p);
		vcy = func_intrpol(vay, vby, p);
		vcz = func_intrpol(vaz, vbz, p);


  - interpolate about rotation matrix elements (Cube-C2)
	When rotation matrix of Cube-A is MA(=ma[3][3]) and
	rotation matrix of Cube-B is MB(=mb[3][3]), a temporary matrix 
	MT(=mt[3][3]) would be given as follows:

		mt[i][j]= func_intrpol(ma[i][j], mb[i][j], p);
		 (i,j=0..2)

	As matrix MT might be distorted, rotation matrix of Cube-C2 (MC)
	would be obtained:

		MC = NormalMatrix(MT);

	This sample program selects which axes are used for NormalMatrix
	to get reduce error.

  - interpolate around the rotation axis

	When rotation matrix of Cube-A is MA(=ma[3][3]) and
	rotation matrix of Cube-B is MB(=mb[3][3]):

	ME = EigenMatrix of ( MA^t MB )

	      {1,       0,      0}
	MT =  {0,  cos(k), sin(k)}
	      {0, -sin(k), cos(k)}

	MC = MA ME MT (ME^t)

	Sample "arot" shows same way under MA is unit matrix.
