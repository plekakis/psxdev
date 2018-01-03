$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$
Mouse interface sample program

Summary

In this sample program, a ball is made to move with the motion of the
mouse. The left mouse button moves the ball to the default left position, 
and the right button moves the ball to the right position. The current 
mouse position is displayed on the screen in real time.

Description of the program

As shown in the function main() in sample program file main.c, function 
InitPAD() is called, and then function InitMouse() is called. SenseMouse(), 
SetMouse() and RengeMouse() initialize respectively the sensitivity of 
the mouse, the position of the mouse, and the range of movement of the 
mouse. Then, MouseRead() is called for each VSync so that the current 
mouse position is obtained. The amount of mouse displacement between 
Vsyncs is obtained by calling MouseState().

