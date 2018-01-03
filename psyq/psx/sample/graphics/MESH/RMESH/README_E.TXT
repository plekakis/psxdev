$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR ROUND MESH FUNCTION(RotMeshPrimR_?)
	
                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


<Description>
	
	This sample program performs following:
	 - Applies coordinate conversion and perspective conversion to the 2 dimensional
           mesh data.
         - Generates packets.
         - Link to OT.
	The round mesh data is specified by the structure "TMESH".


		    2-------3
		   / \     / \
		  /   \   /   \
		 /     \ /     \
		1-------0-------4
			 \     /
			  \   /
			   \ /
			    5

		Figure1. Round Mesh Data


	typedef struct {
        	SVECTOR         *v;             /*shared vertices*/
        	SVECTOR         *n;             /*shared normals*/
        	SVECTOR         *u;             /*shared texture addresses*/
        	CVECTOR         *c;             /*shared colors*/
        	u_long          len;            /*mesh length(=#vertex)*/
	} TMESH;

		Figure 2.TMESH Structure

	In Figure 1,
		len = 6
		v,n,u,c are arrays with elements in order as above figure 1.

	The tuto0 displays various modes at one time.
	The number of the polygons displayed is fixed.
	G3, F3, FT3, GT3
	GC3,FC3,FCT3,GCT3,T3

<How to Execute>
        psymake    	compile
        psymake load    load data
        run tuto0       run

<Controller Button Assignment>
	tuto0
        	Up Directional Button   :Translation in Y-axis
        	Down Directional Button :Translation in Y-axis
        	Right Directional Button:Translation in X-axis
        	Left Directional Button :Translation in X-axis
        	Square Button           :Rotation about Y-axis
        	Cross Button            :Rotation about X-axis
        	Triangle Button         :Rotation about X-axis
        	Circle Button           :Rotation about Y-axis
        	L1 Button               :Translation in Z-axis
        	L2 Button               :Rotation about Y-axis & Depth cueing OFF
        	R1 Button               :Translation in Z-axis
        	R2 Button               :Rotation about Y-axis & Depth cueing ON
        	(Start Button)&(Select Button) :backface clip ON/OFF
        	(R2 Button)&(L2 Button)&(Select Button) :Quit


