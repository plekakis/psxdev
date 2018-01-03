$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR 2 DIMENTIONAL MESH FUNCTIONS(RotMeshPrimQ_T)	

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


<Description>
	This sample program performs following:
	- Applies coordinate conversion and perspective conversion to the 2 dimensional
          mesh data.
        - Generates packets.
        - Link to OT.
	The 2 dimensional mesh data is specified by the structure "QMESH".


		0---1---2---3---4---5---6---7---8
		|   |   |   |   |   |   |   |   |
		9---10--11--12--13--14--15--16--17
		|   |   |   |   |   |   |   |   |
		18--19--20--21--22--23--24--25--26
		|   |   |   |   |   |   |   |   |
		27--28--29--30--31--32--33--34--35

		    2 dimensional mesh data


	typedef struct {
   	SVECTOR         *v;         /*shared vertices*/
        	SVECTOR         *n;         /*shared normals*/
        	SVECTOR         *u;         /*shared texture addresses*/
        	CVECTOR         *c;         /*shared colors*/
        	u_long          lenv;       /*mesh length_V(=#vertex_V)*/
        	u_long          lenh;       /*mesh length_H(=#vertex_H)*/
	} QMESH;

			Structure "QMESH"

	Above,
		lenv = 4
		lenh = 9
		v,n,u,c are arrays with elements in order as above figure.

	In the sample program tuto0 has;
		lenh = 42
		lenv = 43
	In the sample program tuto1 has;
		lenh = 42
		lenv = 46
	
	lenh must be a multiple of 3 since functions with the 3 vertices concurrent
        calculations are used.
        
      	RotMeshPrimQ_T is only for the texture mode and the light calculation will not 
        be done except for the depth queuing. Thus n,c in QMESH will be ignored.
       
	RotMeshPrimQ_T can perform the screen clipping that does not draw polygons        
         outside a clipping area. 
	"tuto0" sets the clip area somewhat smaller than the screen size making the 
         clipping	process easier to see.
	"tuto1" matches the clip area with screen size.

	The number displayed is the calculation time for one frame with unit Hsync.

<How to Execute>
        psymake    	compile
        run tuto0       run

        run tuto1       run
	
<Controller Button Assignment>
	tuto0
        	Up Directional Button   :Depth queuing ON
        	Down Directional Button :Depth queuing OFF
        	Right Directional Button:Translation in X-axis direction
        	Left Directional Button :Translation in X-axis direction
        	Square Button           :Rotation about Y-axis
        	Cross Button            :Rotation about Z-axis
        	Triangle Button         :Rotation about Z-axis
        	Circle Button           :Rotation about Y-axis
        	L1 Button               :Translation in Z-axis direction
        	L2 Button               :backface clip OFF
        	R1 Button               :Translation in Z-axis direction
        	R2 Button               :backface clip ON
        	Select Button           :Quit

	tuto1
        	Up Directional Button   :Translation in Y-axis direction
        	Down Directional Button :Translation in Y-axis direction
        	Right Directional Button:Translation in X-axis direction
        	Left Directional Button :Translation in X-axis direction
        	Square Button           :Rotation about Y-axis
        	Cross Button            :Depth queuing OFF
        	Triangle Button         :Depth queuing ON
        	Circle Button           :Rotation about Y-axis
        	L1 Button               :Translation in Z-axis direction
        	L2 Button               :backface clip OFF
        	R1 Button               :Translation in Z-axis direction
        	R2 Button               :backface clip ON
        	Select Button           :Quit

_
