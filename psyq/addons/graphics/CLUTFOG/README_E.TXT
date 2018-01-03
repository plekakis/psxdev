$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$


-----------------------------------------------------------------------------
							1995.09.29 SCE
	Sample program for CLUT fog texture mapping

*tuto0
	tuto0 makes depth queued CLUT by each frame, and loads it to the
	frame buffer. This method does not need more extra CLUT memroy
	area on the frame buffer, and it can make only 1 depth by each 
	frame, each CLUT.

*tuto1
	tuto1 makes some depth queued CLUT patterns at the first time.
	Each primitive change its CLUT entry according to its Z value
	to simulate foggy space. This method can't be applied to GsSortObj*()
	in libgs.

*tuto2
	tuto2 makes some depth cued CLUT patterns on the frame buffer
	at the first time. DR_MOVE primitive moves matched CLUT
	patterns into entryed (using) CLUT area. The DR_MOVE primitive 
	is entryed on Ordering Table. This method changes CLUT during
	the drawing, so it can be applied to GsSortObj*() in libgs.
	This method is faster than tuto0 but it needs more CLUT memory area.

*controller assign
	Left cross key		move up/down/right/left 
	L1,L2			foreward/backwaord
	R1,R2			rotation (except tuto0)
	Right cross key		rotation (except tuto0)

*compile
	>psymake tuto0
	>psymake tuto1
	>psymake tuto2

*load
	>psymake load
	>run tuto0

	>psymake load
	>run tuto1

	>psymake load
	>run tuto2


*relative infomation
	The correspondings between Z (otz) and depth cue parameter (p)
	can be obtained by function otz2p() or p2otz().

			Copyright (C) 1996 Sony Computer Entertainment Inc.
