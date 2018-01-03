$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

	SAMPLE PROGRAM FOR ROUND MESH FUNCTION(RotMeshPrimR_?)
	

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


<How to operate>
	tuto0
		PADRup,PADRdown		Rotation about X-axis
		PADRleft,PADRright	Rotation about Y-axis
		PADLup,PADLdown		Translation in Y-axis 
		PADLleft,PADLright	Translation in X-axis
		PADL1,PADR1		Translation in Z-axis
		PADR2,PADL2		Rotation about Y-axis & 
					Depth cueing ON/OFF
		PADstart,PADselect	backface clip ON/OFF
		PADR2&PADL2&PADselect	Terminate

