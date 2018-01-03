$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


ammonite: a combined sample of animated MIMe

<description>
	This data shows how to control MIMe parameters with general-purpose
	animation primitives.
	Please use anim/animview in HMD sample program directory to run.

	The following steps create this data:

	1. Based on the model is created by N-Geometry of Nichimen Graphics
	   N-World, by the described method in "Self Study Guide".
	2. The model is exported as Alias|wavefront OBJ format.
	3. The OBJ file is imported into LightWave 3D.
	4. Details of the model, i.e. texture and colors, are modified.
	5. Three variations of the ammonite are created as MIMe keys.
	6. "Animation Saver for PlayStation" generated three HMD files.
	7. "mkmime" is used to generate an HMD file contains MIMe information.
	8. "xhmd" is used to dis-assemble.
	9. Animation primitives are added to the LAB file created in step 8.
	   These animation primitives control coordinates and MIMe parameters.
