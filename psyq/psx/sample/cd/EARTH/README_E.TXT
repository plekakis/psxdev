$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
			libcd tutorial

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	This sample program uses movie image as a 16bit mode texture
	pattern. This program does not need whole 1 frame image buffer
	in the main memory.

	Animation is executed followin functions in "cdadd.c":
	
		animInit();
		animPoll();

	animPoll() consumes long CPU time because it calls DecDCTvlc()
	in it. Therefore it can not be called in the callbacks such as
	VSyncCallback().

	To assure to execute LoadImage immediately, the background
	drawing process is interrupted by BreakDraw(). But BreakDraw()
	is effective only for background DrawOTag() (and its higher
	function GsDrawOt()). Therefore please be careful if you use
	other LoadImage() in the background.
	
	See following sample code for similar operations:

		sample/graphics/pmd
		sample/cd/movie
		sample/cd/str3d
<How to Execute>
	
	>resetps 1
	>psymake load
	>psymake tuto0
	>run earth

<Controller Button Assignment>
        Up and Down directional Button  :  increase/decrease objects
        L1 Button                       :  move far
        L2 Button                       :  move near
