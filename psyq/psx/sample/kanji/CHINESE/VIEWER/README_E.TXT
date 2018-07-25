$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

viewer	Chinese KANJI Code Viewer Program

                             Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                           All rights reserved.


<explanation>
	This sample is a viewer for Chinese KANJI characters.
	All of Chinese KANJI fonts can be displayed.

	Two different viewers for 15dots font data and 8dot font data are 
	made by one source code. 

	There are two different type of the 15dot font data.
	One is the "KANTAIJI" and another is the "HANTAIJI".

	By changing the data to download, all of each font data can be
	displayed.
	Also, the viewer for the 15dot font data magnifys the selected
	character and displays the Shift-JIS code and CX(GB) code.

<how to run>
	psymake			compile view8 and view15

	psymake loadkan		load "KANTAIJI" font data and code table
	run view15		run viewer for the 15dot font data

	psymake loadhan		load "HANTAIJI" font data and code table
	run view15		run viewer for the 15dot font data

	psymake loadhalf	load half size font data
	run view8		run viewer for the 8dot font data

<key assign>
	left direction buttons	:	Scrolling upward and downward
	right direction buttons	:	transfer the cursor	
	L1/L2/R1/R2 buttons	:	magnify the selected character and
					display the code number

	START & SELECT		:	quit

	Keeping any key pushed will increase the speed in 3 steps; 
        standard, double, 4 times.
