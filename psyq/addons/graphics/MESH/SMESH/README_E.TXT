$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

	SAMPLE PROGRAM FOR STRIP MESH FUNCTION(RotMeshPrimS_?)
	
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
	F3->G3->FT3->FC3->GC3->GT3->FCT3->GCT3->T3Ñ¢
	Ñ§ÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑ£
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

<How to operate>
	tuto0
                 PADRup,PADRdown		Rotation about X-axis
		PADRleft,PADRright	Rotation about X-axis
		PADLup,PADLdown		Change polygon mode
		PADLleft,PADLright	Change polygon number
		PADL1,PADR1		Translation in Z-axis
		PADR2,PADL2		Depth queuing ON/OFF
		PADstart,PADselect	backface clip ON/OFF
		PADL1&PADR1&PADselect	Stop hopping
		PADR2&PADL2&PADstart	Terminate

	tuto1
		PADRup,PADRdown		Rotation about X-axis
		PADRleft,PADRright	Rotation about Y-axis
		PADLup,PADLdown		Translation in Y-axis 
		PADLleft,PADLright	Translation in X-axis
		PADL1,PADR1		Translation in Z-axis
		PADR2,PADL2		Depth queuing ON/OFF
		PADstart,PADselect	backface clip ON/OFF
		PADR2&PADL2&PADstart	Terminate

	tuto2
		PADRup,PADRdown		Rotation about X-axis ( common center Åj
		PADRleft,PADRright	Rotation about Y-axis ( common center Åj
		PADL1,PADR1		Rotation about Z-axis ( common center Åj
		PADR2,PADL2		Translation in X-axis direction
		PADLup			Increment icosahedron
		PADLdown			Decrement icosahedron
		PADLleft			Increment icosahedron with texture 
mapping
		PADLright		Decrement icosahedron with texture 
mapping
		PADLup&PADh		Depth queuing ON
		PADLdown&PADh 		Depth queuing OFF
		PADLleft&PADh		Depth queuing FAR
		PADLright&PADh		Depth queuing NEAR
		PADk			Terminate
_

