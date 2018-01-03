$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


tri3: animated triceratops

<description>
	This sample data shows a triceratops that is animated by HMD animation
	primitive. Please use anim/tuto0 in HMD sample program directory to run.

	This data is created by the following steps:

	1. Bones and animation are added to "triceratops", a sample data of
	   LightWave3D, by Layout Editor of LightWave3D.
	2. Old-RSD files are created by PlayStation plugin for LightWave3D
	   developed and distributed by NewTek/D-Storm.
	3. tri.cod, tri.mot and tri.pvt are created by "lws2rsd" from LWS file
	   created in step 1.
	4. "rsdup" is used to create new-RSD format files (tri.rsd,
	   tri0.ply, tri0.mat and tri0.grp) from old-RSD created in step 2.
	5. "meshconv" is used to create tri.msh.
	6. "rsd2hmd" created tri.hmd from the above files.
