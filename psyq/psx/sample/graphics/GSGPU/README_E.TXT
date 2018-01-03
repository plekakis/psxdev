$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

			libgpu and libgs

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	- Here are samples which use both libgs functions and libgte
	  functions. Each sample display 3D object handled by libgs and
	  2D sprites drawn by low level primitives. The left-upper
	  corner of the 2D sprites are calculated by libgte.

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.

        ============================================================
	program		contents
        ------------------------------------------------------------
	tuto0		append lowlevel primitives on the OT libgs
			controls. 	
	tuto1		append a complex TMD object handled by libgs
			on the rare OT.	
        ------------------------------------------------------------

<How to Execute>
	
	>psymake main
	>psymake load
	>run main

<Controller Button Assignment>
        Directional Button      :  move cursor
        Circle Button           :  execute
        Select Button           :  exit

<Remarks>
	- DEMO button indicates to start demonstration program.
	- HELP button indicates to display a help window.
	  Key assign of each demonstration is shown on this help window.
