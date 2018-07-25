$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

mime: MIMe sample using HMD

                             Copyright(C) 1997 Sony Computer Entertainment Inc.
                             All rights reserved.
<Descriptions>
	This is an example of MIMe in which the HMD data format is
	used. 

	Multiple interpolation of joint bending (Joint-MIMe) is
	performed in the sample using the Triceratops model. Axes-MIMe
	and RPY-MIMe can be used as the interpolation methods. The
	Triceratops is a one-skin model with a joint structure.

	Multiple interpolation of vertices and normal vectors, which is
	the conventional type of interpolation(Vertex/Normal-MIMe), is
	performed in the sample using the Puyode model. Puyode does
	not have a joint structure.


<How to Execute>
	
	> psymake all		compile

	for Model #0 (Joint-Axes-MIMe sample)
	> psymake load0
	> run tuto0

	for Model #1 (Vertex-MIMe sample)
	> psymake load1	
	> run tuto0

	for Model #2 (Joint-RPY-MIMe sample)
	> psymake load2
	> run tuto0

	Run tuto1 (instead of tuto0) to use analog controller.

	for Model #3 (Cloned MIMe sample)
	> psymake load3
	> run tuto2

	For Model #3, use tuto2 instead of tuto0 or tuto1 to use two
	controllers.


<Controller Button Assignment>
	L1/L2/R1/R2/Directional Button: operate MIMe parameters
		L1 Button:			MIMe key 1
		L2 Button:			MIMe key 2
		R1 Button:			MIMe key 3
		R2 Button:			MIMe key 4
		Left Directional Button:	MIMe key 5 (except for Model #1)
		Right Directional Button	MIMe key 6 (except for Model #1)
		Up Directional Button:		MIMe key 7 (except for Model #1)
		Down Directional Button:	MIMe key 8 (except for Model #1)
		(Each MIMe weight is displayed on the screen)
	
		Circle/Triangle/Square/Cross Button: rotate the object
		SELECT, START: translate the object along the Z-axis

<explanation>

- Precautions when creating a program for handling HMD data including MIMe data
	To be able to handle HMD that has a MIMe primitive,
	Initialization must be performed using the function
	GsInitRstVtxMIMe()/GsInitRstNrmMIMe() to do Reset-Vertex/Normal MIMe.
	This is in addition to normal HMD initialization.
	The function GsInitRstVtxMIMe()/GsInitRstNrmMIMe() must be called
	after GsScanUnit(), but before GsSortUnit().

	If the MIMe parameters (interpolation weighting coefficients)
	are contained in the program, and not in the HMD, the address
	where the parameters are placed must be specified (please refer
	to "tuto2.c").

	Within sample programs, these initializations are performed in the
	model_init() procedure.


<Postscript>
	The model data "trirpy.hmd" and "triaxes.hmd" used in this
	sample were created by Viewpoint DataLabs (USA) based on data
	from Newtek Incorporated (USA).
