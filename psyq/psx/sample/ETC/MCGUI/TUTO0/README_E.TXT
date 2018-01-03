$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
$PSDocld$

Tutorial for the Memory Card GUI Module

                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


TUTOO:
======

<Description of the Sample>
	This is a sample program which uses the Memory Card GUI Module.
	
	The following card data is saved to or loaded from the Memory 
	Card in Memory Card slot 1 or 2. 
	
	File Name [HAND]
	No. of Blocks Used: 1
	Animation: 3 types ("Scissors", "Paper" and "Stone" at a mora)

<How to execute>
	>psymake	Compiling
	>psymake load	Data download
	>run main	Run

<Controller Button Assignment>

	[BALLS Screen]

	Up/Down directional button:  	Select "Save" or "Load" 
	Circle button:  		Start execution
	Select button :  		Exit

	[McGUI Screen]

	Up/Down directional button:     Select Memory Card slot
	Right/Left directional button:  Select "Yes" or "No"
	Circle button:  		Start execution
	Cross button:  			Cancel

<Note>
	The BG mode can be changed by changing the value of 
	macro BG (#define BG.)
	
	BG = 0:  Scroll BG created by a tile pattern of mcgui.tim.
	BG = 1:  Still picture BG created by mcgui_bg.tim.

