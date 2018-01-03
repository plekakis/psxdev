$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

Sample program for Controller library (libpad)

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<Descriptions>

	This program shows how to use the functions of Controller library
	(libpad): how to operate two types of actuator, switching the 
	terminal type, Lock/Unlocking of terminal type switch, and the like. 
	This program also displays the actuator information of Controller 
	obtained by the actuator query function on the screen.  
	Moreover, this program uses a new library function to 
	suspend/resume 	communication with the controller for each 
	PlayStation port.

	  
	* Explanation of Screen Display

	[MULTI TAP]
		[MULTI TAP]: controllers connected through multi tap
		[DIRECT]: controllers connected directly to controller terminal

	1A [EX]=7
		Controller(Dual Shock) with controller mode ID =7 is 
		connected to Multi tap port A of PlayStation port 1. 
		Two other types are: 
		[NONE]: no controller connected
		[CTRL]: controller(not Dual Shock) connected

	G=0300,156
		displayed only when connected gun type controller
		(terminal type is 3). shows location data on screen

	B=FFFF
		Button state (for all 4 bits 1:Release  0:Pushed)

	ST=80,80,80,80
		Analog level (4 channels)
		Displayed only when #define MULTITAP 0 

	ID 4,7
		Controller IDs currently supported are 4 and 7

	AC(1,2,0,10)=0 (1,1,1,20)=90
  	Actuator 0 (1,  : continuous-rotary-vibrator
		   2,  : fast-rotation
		   0,  : data lengh=1 bit
		  10)  : current supply = 100mA 
       		   =0  :current value = 0
	Actuator 1 (1,   : continuous-rotary-vibrator
		   1,   : slow-rotation
		   1,   : data lengh=1 byte			
		   20)  : current supply = 200mA 
		   =90  :current value = 90

	CM 0,1
		The actuator combination 0 and 1 can be controlled
		simultaneously.

	SW ULK
		Terminal type switch is available(unlocked)
		Another value is LK: not available(locked)

< How to execute > 
	>psymake	Compile
	>run main	Execution

< Controller Button Assignment >
	L1: actuator 0 level -1
	L2: actuator 0 level +1
	R1 : actuator 0 level -10
	R2 : actuator 0 level +10
	Circle Button: Lock/Unlock terminal type switch 
	Left Directional Button: set terminal type to 16-button controller
	Right Directional Button: set terminal type to Analog controller
	

< Remarks >
	Although some information is truncated on the screen when
	more than five controllers are connected due to the limitation 
	of displayable characters in FntPrint(),
	communication with the controllers are in operation.
	

