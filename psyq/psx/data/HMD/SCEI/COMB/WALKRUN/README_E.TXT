$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


walkrun: walking and running legs with animated Joint MIMe

<description>
	This data shows the following points:
	1. Automated Joint MIMe with general purpose animation.
	2. Motion switching (SID=1:run, SID=2:walk).
	Please use anim/animview in HMD sample program directory to run.

	This data is created by the following steps:

	1. "mime.lab" is generated from "mime/legs/axes.hmd" by xhmd.
	2. "MIMePr" section and general purpose animation is added for
	   "mime.lab" to create "run.lab" and "walk.lab".
	3. That two files ("run.lab" and "walk.lab") are combined into
	   "switch.lab" with SID switching.
