$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1996.01.27
	
				  mat
	
	    Copyright (C) 1994-1996, Sony Computer Entertainment
			    All right reserved
	

	This sample displays 2D sprites int the 3D coordinate system.
	The texture pattern mapped on 2D sprites are changed frame by
	frame so that sprite patterns animate. The animated
	robot is called 'Matchang', from which this directory name comes.
	
	All of animation patterns are loaded on the frame buffer. This
	patters consumes 4 pages of 4bit texture pattern.

	A textured quadangle primitive (POLY_FT4) is used for the
	animated sprite. This primitive allways have to be located
	the parallel to the screen. So the local-screen matrix of the
	sprite is forced to be changed to unit matrix.

	See following files for detail description

		doc/jp/gte/{abstract.txt,function.txt}


