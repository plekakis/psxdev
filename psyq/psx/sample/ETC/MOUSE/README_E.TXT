$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       
Mouse interface sample program

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	Summary
	In this sample program, a ball is made to move with the motion of 
	the mouse. The left mouse button moves the ball to the default left 
	position, and the right button moves the ball to the right position. 
	The current mouse position is displayed on the screen in real time.

	Description of the program
	As shown in the function main() in sample program file main.c, function 
	InitPAD() is called, and then function InitMouse() is called. 
	SenseMouse(), SetMouse() and RengeMouse() initialize respectively the 
	sensitivity of the mouse, the position of the mouse, and the range of 
	movement of the mouse. Then, MouseRead() is called for each VSync so 
	that the current mouse position is obtained. The amount of mouse 
	displacement between Vsyncs is obtained by calling MouseState().

<How to Execute>
        psymake 	compile
        run test       	run

<Controller Button Assignment>
        Right button	:	move to the default right possition
        Left button 	:	move to the default left posiotion
        
<Remarks>
