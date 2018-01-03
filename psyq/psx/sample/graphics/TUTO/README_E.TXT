$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

			libgpu tutorial

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	- Here are simple tutorial samples using low level library
	  (libgpu) 

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.


	---------------------------------------------------------------------
	file		| contents
	----------------+----------------------------------------------------
	tuto0		|display simple sprites
	tuto1		|Ordering Table for display priority control
	tuto2		|simple example for libgte
	tuto3		|draw simple 3D object (cube).
	tuto4		|add lighting calculation to "tuto3"
	tuto5		|add texture mapping to "tuto3"
	tuto6		|add depth queue (fog effect) to "tuto3"
	tuto7		|draw many cubes with hierachy coordinate system
	tuto8		|scroll background plane (one dimensional)
	tuto9		|cell type scrolling BG (two dimensional)
	tuto10		|3D cell type BG (bird view)
	tuto11		|pseudo mosaic effect
	tuto12		|pseudo line scroll effect
	tuto13		|multi window operation
	---------------------------------------------------------------------

	Each program share following source code.
	
	---------------------------------------------------------------------
	file		|contents
	----------------+----------------------------------------------------
	init.c		|initialize graphic environment
	cube.c		|library to draw several type 3D cube
	lscr.c		|library to do line scroll
	---------------------------------------------------------------------
	
	Each program share following texture pattern.
	Texture pattern data can be compiled and linked with other C
	source code .

	---------------------------------------------------------------------
	file		|contents
	----------------+----------------------------------------------------
	a)mat0tex.c	|4bit 64x64 sprite pattern (page #0)
	  mat1tex.c	|4bit 64x64 sprite pattern (page #1)
	  mat2tex.c	|4bit 64x64 sprite pattern (page #2)
	  mat3tex.c	|4bit 64x64 sprite pattern (page #3)
	----------------+----------------------------------------------------
	b)far0tex.c	|8bit 256x256 texture pattern
	  far1tex.c	|8bit 256x256 texture pattern
	  near0tex.c	|8bit 256x256 texture pattern
	  near1tex.c	|8bit 256x256 texture pattern
	  wintex.c	|8bit 256x256 texture pattern
	----------------+----------------------------------------------------
	c)bgtex.c	|4bit 32x32 BG cell pattern
	----------------+----------------------------------------------------
	d)balltex.c	|4bit 16x16 ball pattern with 32 CLUTs
	---------------------------------------------------------------------
	
	Texture pattern is stored in the predifined array style in the
	following format.
	
	a),c)
	0x00-0x7f	4bit CLUT (only top 32byte is used)
	0x80-		256x256 4bit texture page image
	
	b)
	0x00-0x7f	8bit CLUT (only top 64byteis used)
	0x80-		256x256 8bit texture page image

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
