$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1996.01.27
	
				  rgb24
	
	    Copyright (C) 1994-1996, Sony Computer Entertainment
			    All right reserved
	
	
	This sample uses the frame buffer as 24bit mode and display
	24bit image dtata.  
	In 24bit mode, each RGB color has 8bit (256colors) width
	(total 3byte), Therefore the displayed pixel postion is not
	same as frame buffer X address.
	GPU cannot draw in this mode but simple frame buffer access
	such as LoadImage,StoreImage,MoveImage,ClearImage is available.

	See following files for detail description

		doc/jp/gpu/{abstract.txt,function.txt}


