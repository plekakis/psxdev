$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR STRIP MESH FUNCTION(RotMeshPrimS_?)
	
                     Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.

<Description>
	This sample program performs following:
	 - Applies coordinate conversion and perspective conversion to the 2 dimensional
           mesh data.
         - Generates packets.
         - Link to OT.
	The strip mesh data is specified by the structure "TMESH".


		    1-------3-------5-------7-------9
		   / \     / \     / \     / \     /
		  /   \   /   \   /   \   /   \   /
		 /     \ /     \ /     \ /     \ /
		0-------2-------4-------6-------8

		Figure 1.trip Mesh Data


	typedef struct {
        	SVECTOR         *v;             /*shared vertices*/
        	SVECTOR         *n;             /*shared normals*/
        	SVECTOR         *u;             /*shared texture addresses*/
        	CVECTOR         *c;             /*shared colors*/
        	u_long          len;            /*mesh length(=#vertex)*/
	} TMESH;

		Figure 2.TMESH Structure

	In Figure 1,
		len = 10
		v,n,u,c are arrays with elements in order as above figure 1.


	tuto0 displays polygons in various modes.
	Number of polygons displayed is variable.
	F3->G3->FT3->FC3->GC3->GT3->FCT3->GCT3->T3-+
	 +-----------------------------------------+
	The number displayed is the number of polygons being drawn per second.
	For each mode, when the number of polygons is MAX, 60 frames/second may not be 
	accomplished depending on the compiler type, compiler options, and other
	conditions.
 
	The tuto1 displays various modes in one time.
	The number of the polygons displayed is fixed.
	G3, F3, FT3, GT3
	GC3,FC3,FCT3,GCT3,T3

	The tuto2 is an example of drawing a icosahedron using the following;  
	RotMeshPrimS_F3
	RotMeshPrimR_F3 
	RotMeshPrimS_FT3
	RotMeshPrimR_FT3

<How to execute>
        psymake    	compile
        psymake load    load data
        run tuto0       run

        psymake load1   load data
        run tuto1       run

        psymake load2   load data
        run tuto2       run

<Controller Button Assignment>
	tuto0
        	Up Directional Button   :Change polygon mode
        	Down Directional Button :Change polygon mode
        	Right Directional Button:Change polygon number
        	Left Directional Button :Change polygon number
        	Square Button           :Rotation about Y-axis
        	Cross Button            :Rotation about X-axis
        	Triangle Button         :Rotation about X-axis
        	Circle Button           :Rotation about Y-axis
        	L1 Button               :Translation in Z-axis
        	L2 Button               :Depth queuing OFF
        	R1 Button               :Translation in Z-axis
        	R2 Button               :Depth queuing ON
        	(Start Button)&(Select Button)	:backface clip ON/OFF
        	(L1 Button)&(R1 Button)&(Select Button)	:Stop hopping
        	(L2 Button)&(R2 Button)&(Start Button)	:Quit

	tuto1
        	Up Directional Button   :Translation in Y-axis
        	Down Directional Button :Translation in Y-axis
        	Right Directional Button:Translation in X-axis
        	Left Directional Button :Translation in X-axis
        	Square Button           :Rotation about Y-axis
        	Cross Button            :Rotation about X-axis
        	Triangle Button         :Rotation about X-axis
        	Circle Button           :Rotation about Y-axis
        	L1 Button               :Translation in Z-axis
        	L2 Button               :Depth queuing OFF
        	R1 Button               :Translation in Z-axis
        	R2 Button               :Depth queuing ON
        	(Start Button)&(Select Button)	:backface clip ON/OFF
        	(L2 Button)&(R2 Button)&(Start Button)	:Quit

	tuto2
        	Up Directional Button   :Increment icosahedron
        	Down Directional Button :Decrement icosahedron
        	Right Directional Button:Decrement icosahedron with texture mapping
        	Left Directional Button :Increment icosahedron with texture mapping
        	Square Button           :Rotation about Y-axis ( common center )
        	Cross Button            :Rotation about X-axis ( common center )
        	Triangle Button         :Rotation about X-axis ( common center )
        	Circle Button           :Rotation about Y-axis ( common center )
        	L1 Button               :Rotation about Z-axis ( common center )
        	L2 Button               :Translation in X-axis direction
        	R1 Button               :Rotation about Z-axis ( common center )
        	R2 Button               :Translation in X-axis direction
        	(Left Directional Button)&(Start Button)	:Depth queuing FAR
        	(Right Directional Button)&(Start Button)	:Depth queuing NEAR
        	(Up Directional Button)&(Start Button)		:Depth queuing ON
        	(Down Directional Button)&(Start Button)	:Depth queuing OFF
        	(Select Button)		:Quit


