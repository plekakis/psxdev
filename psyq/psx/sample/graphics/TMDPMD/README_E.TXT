$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       
				TMD and PMD

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	- Here are sample programs to handle TMD format using lowlevel
	  functions. The decode programs use DMPSX GTE macro for faster
	  calculateion. 

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.

	PMD is 3D primitive data format for fast drawing. Since PMD
	data includes GPU primitive structure, fewer memory access is
	required for primitive making.
	
	rotPMD*() functions decodes PMD format. they only change x,y
	member of the primitve and do not touch r,g,b and u,v member.
	Therefore PMD does not have normal vector information for
	lighting calculation. Please use TMD foramt if real-time
	lighting is needed.
	
<How to Execute>
	
	>psymake load
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
