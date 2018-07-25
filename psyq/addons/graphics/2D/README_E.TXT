$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1996.01.27
	
				  2D
	
	    Copyright (C) 1994-1996, Sony Computer Entertainment
			    All right reserved
	

	This sample is for texture mapping on the free curved surface
	using mesh vertex structure. PlayStation has only linear
	texture mapping hardware, but if you divide polygon into small
	mesh primitives and calculate exact vertex point on each mesh
	point (representive point), it is possible to make texture
	mapping onto any surface. This dividing approach enabls
	non-distorted texture mapping with perspective translation.

	See following file for more information:

		doc/jp/gpu/{abstract.txt,function.txt}
		doc/jp/gte/{abstract.txt,function.txt}


