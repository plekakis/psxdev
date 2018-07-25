$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


legs: a pair of legs with Joint MIMe

<description>
	This data shows the following points:
	1. Joint MIMe.
	2. Clone an object.
	3. A sample result of "meshconv" command.
	Please use mime/tuto1 in HMD sample program directory to run.

	This data is created by the following steps:

	1. Old-RSD files are created from a sample data of LightWave 3D
	   by PlayStation plugin for LightWave3D.
	2. "rsdup" is used to create new-RSD format files from old-RSD
	   created in step 1.
	3. "meshconv" is used to create mesh structure.
	4. Only meshes' colors are changed to red by text editor. 
	5. "rsd2hmd" created a HMD file from the above files.
	6. "xhmd" is used to dis-assemble the HMD file created in step 5.
	7. The result of step 6 is edited by text editor to create MIMe keys
	   and clone a leg (*.lab).
	8. "labp" command created HMD files from *.lab files created in step 7.
	9. "mkmime" is used to generate axes.hmd and rpy.hmd.
