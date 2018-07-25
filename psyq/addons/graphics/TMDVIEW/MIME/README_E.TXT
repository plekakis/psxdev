$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

	main:  sample code for MIMe using GsDOBJ5

[how to run]
	> psymake all
	> psymake load
	> run main

[key assign]

	left direction buttons		transfer XY
	right direction buttons		rotation
	start&select buttons		transfer Z

	R1 keys				MIMe motion-1
	R2 keys				MIMe motion-2
	L1 keys				MIMe motion-3
	L2 keys				MIMe motion-4

[modeling data]

	md0.tmd		Base model		
	md1.tmd		Deformed model for MIMe motion-1
	md2.tmd		Deformed model for MIMe motion-2
	md3.tmd		Deformed model for MIMe motion-3
	md4.tmd		Deformed model for MIMe motion-4
	md0.vdf		Differential vertex file		
	md0.ndf		Differential normal file

	md0.tmd,md0.vdf and md0.ndf are used in sample program.

	Differential files (VDF & NDF) are created using mimefilt as below.

@	mimefilt -n md0.tmd md1.tmd md2.tmd md3.tmd md4.tmd@	

	(see Programer Tool: mimefilt)

[explanations]

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

	 ( P means the vertex or normal set in TDM.)
	 (dP means vdf or ndf.)

	      _n
	(P0 + > Wi * dPi) is calculated in gteMIMefunc, using GTE.
	      ~i=1
