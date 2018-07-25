$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

			libpress tutorial

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	- Here are simple tutorials about image decompression library
	(libpress). 

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.


	---------------------------------------------------------------------
	file		|contents
	----------------+----------------------------------------------------
	tuto1		| simple VLC decode and MDEC on memory decompression
	tuto2		| paralell execution of LoadImage() and DecDCTout()
	tuto3		| simple on-memory movie operation
	tuto4		| handshake using callback
	tuto5		| parallel execution of LoadImage() and DecDCTout()
			| using callback.
	tuto6		| complete background on-memory movie decompression
	tuto7		| fine tune-up for parameters
	---------------------------------------------------------------------

	Each program uses follwing data file (bitstream data)

	---------------------------------------------------------------------
	file		|contents
	----------------+----------------------------------------------------
	msiro.bs	|still image (256x160)
	msiro/*.bs	|animated image (256x176)
	---------------------------------------------------------------------

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
