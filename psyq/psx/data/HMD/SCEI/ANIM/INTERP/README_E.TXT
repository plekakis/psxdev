$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     				   All rights reserved.


interp: an animation with three types of interpolation

<description>
	This data shows Linear, Bezier and B-Spline interpolated animation.
	Please use anim/animview in HMD sample program directory to run.

	The following steps create this data:

	1. 3D Studio MAX and GAMUT-PSm are used to create Old-RSD files
	2. "rsdup" is used to create new-RSD format files from old-RSD
	   created in step 1.
	3. "rsd2hmd" created a HMD file from the above files.
	4. "xhmd" is used to dis-assemble the HMD file created in step 3.
	5. interp.lab is created by text editor. This file contains non-model
	   data, for example animation and coordinates.
	6. To combine with interp.lab, files created in step 4 are edited by
	   text editor and saved as *.inc files.
	7. "labp" command created HMD files from interp.lab file created in
	   step 5.

	In step 1, another version of Old-RSD files are created by Nichimen
	Graphics N-World and PlayStation Express.
	In default setting, interp.lab includes *.inc files that are created by
	3D Studio MAX, but *.inc files created by N-World will be included if
	you changed USE_NWORLD_OBJS to 1 and USE_3DMAX_OBJS to 0 in the
	beginning of interp.lab.

	You will see four blue spheres on the screen. These spheres are
	located at four key frames.
	And data created by 3D Studio MAX, be.inc ("be" in Japanese),
	ri.inc ("ri" in Japanese) and su.inc ("su" in Japanese) are
	interpolated by Bezier, Linear and B-Spline.
	Data created by N-World, bezier.inc (Z), linear.inc (L) and
	bspline.inc (B) are interpolated by its own name.
