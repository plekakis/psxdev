$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


twinp: a pair of "puyo"

<description>
	This data shows the following points:
	1. Clone an MIMe object.
	2. A sample usage of "MIMeID".
	Please use mime/tuto2 in HMD sample program directory to run.

	This data is created by the following steps:

	1. "xhmd" is used to dis-assemble ../puyo.hmd.
	2. The result of step 1 is edited by text editor to create MIMe keys,
	   clone an object and change "MIMeID" that is used to identify
	   control pad (*.lab).
	3. "labp" command created HMD files from *.lab files created in step 2.
	4. "mkmime" is used to generate twinp.hmd.
