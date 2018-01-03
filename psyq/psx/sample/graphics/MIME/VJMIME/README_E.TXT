$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

vjmime: Joint MIMe Animation of Articulated Model with Vertex MIMe

                  Copyright(C) 1996,1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
This is a sample program for manipulating an articulated model using 
the vertex multiple inbetweening method( hereafter refer to as 
vertex-MIMe) and/or the joint multiple inbetweening method
(hereafter refer to as joint-MIMe).  

This program demonstrates two kinds of joint-MIMes;

 - axes-MIMe: interpolation by axes 
 - RPY-MIMe: interpolation by roll, pitch, and yaw angles. 

tuto0 shows an example of interpolation using both joint-MIMe and vertex-
MIMe.

tuto1, on the other hand, shows an example of interpolation using only 
joint-MIMe.
tuto1 is able to manipulate states of the model as well.
 

<How to Execute>

  - tuto0
 > psymake all		compile
 > psymake load0	load data
 > run tuto0		run

  - tuto1   
 > psymake all		compile
 > psymake load1	load data
 > run tuto1		run


<Controller Button Assignment>
Controller 1: Used to see how MIMe works with the preset models or 
  edited original models.   
  
Controller 2: Used mainly for editing models (available only for tuto1)


 * Controller 1: 
  L1/L2/R1/R2: MIMe Parameter Manipulation
   L1: MIMe Key 1
   L2: MIMe Key 2
   R1: MIMe Key 3
   R2: MIMe Key 4
   MIMe: parameters for each MIMe are displayed on the screen. 

  Left/Right Directional Button: Toggles preset models, or
     reset to the preset state.

  Circle,Square,Triangle,Cross Button: Changes view point

  START Button: Toggles two kinds of joint-MIMe,axes-MIMe and RPY-MIMe. 
     Currently selected method is displayed on the screen.
  SELECT Button: Dumps the current model state (only for tuto1)
   (Can be used as a preset after adding it to "preset.c")

 * Controller 2  (used only for tuto1):
  Circle,Square,Triangle,Cross,R1,R2 Button: Select Rotation or Translation
  START Button: Rotation Manipulation Mode 
     ("ROTATION" displayed on the screen)
  SELECT Button: Translate Manipulation Mode 
     ("TRANSLATION" displayed on the screen )

  Directional Buttons: Changes edit target
			( One with a cursor is selected target)
	Up/Down Directional Button: Changes models (MIMe1/MIMe2/MIMe3/MIMe4/ALL)
	     When a cursor is on the base model, all modes can be changed
	     simultaneously.
	Left/Right Directional Button: Changes objects 

<Remarks>

* How to use samples

1.After program starts, gradually press L/R buttons( L1,L2 R1, and/or R2)
 
  of Controller 1.
  When you press button(s), in the middle of the screen, you will see 
the 
  outcome of the interpolation of the base and the MIMe-key(s) that 
  corresponds to the button(s) you pressed. For detail of the Controller Button 
  allocation, refer to the section "Controller Button Allocation" of this 
document. 

 Left:  Base 
 Middle: Outcome of MIMe 
 Right:  Each of the four L/R button corresponds to a MIMe key

             +-----------------------------+
             |                             |
             |                   MIMe-key1 |
             |                             |
             |Base Interpolated  MIMe-key2 |
             |Model    Model               |
             |                   MIMe-key3 |
             |                             |
             |                   MIMe-key4 |
             +-----------------------------+

                         Screen


(only for tuto1)
2.Pressing either the Left or Right button of directional buttons
  (four on the left side) of Controller 1, the next preset model is displayed
  in the second line of upper left screen. 

3.Pressing Start button of Controller 1 can toggle between the two joint-MIMes;
  axes-MIMe and RPY-MIMe.   
  Current MIMe selected is displayed in the first line of upper left 
screen. 

(only for tuto1) 
4.You can edit models to create original ones using Controller 2. 
  Select the target by directional button(s) first. Selected 
  target will be displayed with a cursor and the current target ID is 
  displayed in the second line on upper left cornor of the screen.
  Pressing  Cross, Circle, Square or Triangle on the right hand side
  and R1 or R2 buttons can rotate/translate the selected model.
  Use Select button for rotation and Start button for translation. 
  Left/Right directional buttons of Controller 1 resets to the preset state.

 - Articulated Model

 At start up, the model has the following hierarchical structure:

    Parent   Child  Grand-child
    +-----+-+-----+-+-----+
   *|     |*|     |*|     |
    +-----+-+-----+-+-----+
  ^        ^       ^
  Parent Child   Grand-child
          Origin   Origin 

-Program Description

 main.c:  Initialization and main loop necessary for standard 
   drawing(OT drawing)
 tuto0.c: Model manipulation and registration to OT(for tuto0)
 tuto1.c: Model manipulation and registration to OT(for tuto1)
 a_intr.c:  Functions for joint-MIMe (using axes-MIMe)
 rpy_intr.c:  Functions for joint-MIMe (using RPY-MIMe)
 nmime.c:  Functions for vertex-MIMe(for tuto0)
 control.c:  Functions for convolution smoothing
 ctlfc.c:  Table used for convolution
 preset.c:  Table for model location preset
