$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

clutfog: Sample program for CLUT fog texture mapping

                       Copyright(C) 1995-1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.



<Description>
	These samples show four methods of depth queueing using CLUT for
	texture	-mapped polygones.
	See GTE Overview document which contains explanations of depth
	queueing method on PS.



    * tuto0
	tuto0 makes depth queued CLUT by each frame, and loads it to the
	frame buffer. This method does not need more extra CLUT memory
	area on the frame buffer, and it can make only 1 depth by each
	frame, each CLUT.

    * tuto1
	tuto1 makes some depth queued CLUT patterns at the first time.
	Each primitive change its CLUT entry according to its Z value
	to simulate foggy space. This method can't be applied to GsSortObj*()
	in libgs.

    * tuto2
	tuto2 makes some depth cued CLUT patterns on the frame buffer
	at the first time. DR_MOVE primitive moves matched CLUT
	patterns into entryed (using) CLUT area. The DR_MOVE primitive
	is entryed on Ordering Table. This method changes CLUT during
	the drawing, so it can be applied to GsSortObj*() in libgs.
	This method is faster than tuto0 but it needs more CLUT memory area.

    * tuto3
	tuto3 loads CLUT patterns by DR_LOAD primitives on Ordering
	Table. This method changes CLUT during the drawing, so it can
	be applied to GsSortObj*() in libgs. Furthermore, it is as fast
	as tuto2, but it does not require extra CLUT memory area on
	the frame buffer.

<How to Execute>

	* tuto0
	> psymake tuto0		compile
	> psymake load		load data
	> run tuto0		run

	* tuto1
 	> psymake tuto1		compile
	> psymake load		load data
	> run tuto1		run

	* tuto2
	> psymake tuto2		compile
	> psymake load		load data
	> run tuto2		run

	* tuto3
	> psymake tuto3		compile
	> psymake load		load data
	> run tuto3		run

<Controller Button Assignment>
        Directional Buttons   : move up/down/right/left
	L1,L2 Button	      : foreward/backwaord
        Square,Cross,Triangle,Circle Button: rotation (except tuto0)
        R1,R2 Button          : rotation (except tuto0)


<Remarks>
	The correspondings between Z (otz) and depth cue parameter (p)
	can be obtained by function otz2p() or p2otz().

