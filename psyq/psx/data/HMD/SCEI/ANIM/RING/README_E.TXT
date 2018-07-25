$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


ring: a ring that shows almost of all primitive types

<description>
	Please use anim/animview in HMD sample program directory to run.

	The following steps create this data:

	1. LightWave 3D 5.5, AnimationSaver 1.41 and PGLeditor are used to
	   create "ring.lab" with "ring.cfg".
	2. A shell script "conv.sh" is used to convert "ring.lab" to
	   "tiled.lab" that contains primitive types for this sample.
	3. "labp" command created HMD file "tiled.hmd" from "tiled.lab".

	You will find two macro defines "XDRV" and "DIVNUM" in "mac.def".
	These macros allow to change primitive driver bit and parameter of
	DIV or ADV.

	You will see a gray cylinder with letters that shows primitive types
	for each half domes around the cylinder.
