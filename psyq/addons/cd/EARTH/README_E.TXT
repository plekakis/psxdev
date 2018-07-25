$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1996.02.08 
			texture pattern animation
		
		Copyright (C) 1996, Sony Computer Entertainment
			    All right reserved
	

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
	
	See following files for detail description:

		doc/jp/format/pmd.txt
		doc/jp/press/{abstract,function}.txt
		doc/jp/cd/{abstract,function}.txt
	
	See following sample code for similar operations:

		sample/graphics/pmd
		sample/cd/movie
		sample/cd/str3d

