$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       
			Basic Graphics Demonstration

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	- This sample demonstrates basic functions of PlayStation
	  graphics hardware.  

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.

   balls:	
	This is a sample program to display many sprites (ball
	pattern) using  PlayStation architecture.

   diffuse:
	This is a sample program to display 3D primitives using
	PlayStation architecture. This sample displays 10x10x10 square
	planes or 10x10x10 textured ball patterns.

   rgb24:	
	This sample uses the frame buffer as 24bit mode and display
	24bit image dtata.  
	In 24bit mode, each RGB color has 8bit (256colors) width
	(total 3byte), Therefore the displayed pixel postion is not
	same as frame buffer X address.
	GPU cannot draw in this mode but simple frame buffer access
	such as LoadImage,StoreImage,MoveImage,ClearImage is available.

    morph:	
	This sample is for texture mapping on the free curved surface
	using mesh vertex structure. PlayStation has only linear
	texture mapping hardware, but if you divide polygon into small
	mesh primitives and calculate exact vertex point on each mesh
	point (representive point), it is possible to make texture
	mapping onto any surface. This dividing approach enabls
	non-distorted texture mapping with perspective translation.

<How to Execute>
	
	>psymake main
	>run main

<Controller Button Assignment>
        Directional Button      :  move cursor
        Circle Button           :  execute
        Select Button           :  exit

<Remarks>
	- DEMO button indicates to start demonstration program.
	- HELP button indicates to display a help window.
	  Key assign of each demonstration is shown on this help window.

