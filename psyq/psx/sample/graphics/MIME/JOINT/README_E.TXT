$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
joint: Joint-MIMe sample program

                Copyright(C) 1996,1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.

<Descriptions>
     This sample program uses MIMe (Multiple Interpolation Method)
     applied to coordinates to manipulate an object having a
     hierarchical structure.  It demonstrates two interpolation
     methods, namely axes-MIMe and RPY(roll/pitch/yaw-angle)-MIMe.

<How to Execute>
     >psymake all        compile
     >psymake load       load data
     >run tuto0          run

<Controller Button Assignment>
     Controller 1: Used to see how MIMe works with the preset models or
		edited original models.
     Controller 2: Used mainly to edit models

     * Controller 1:
        L1/L2/R1/R2: MIMe Parameter Manipulation
        L1: MIMe Key 1
        L2: MIMe Key 2
        R1: MIMe Key 3
        R2: MIMe Key 4
      Left/Right of Directional Buttons: toggle preset
          models, or reset to the preset state.
      Square,Circle,Cross,Triangle Button: Changes view point
      START: Toggles two kinds of joint-MIMe,
          joint-axes-MIMe and joint-RPY-MIMe.
          Currently selected method is displayed on the screen.
      SELECT: Dumps the current model state (Can be used as a
          preset after adding it to "preset.c")

     * Controller 2:
      R1/R2/Square,Circle,Cross,Triangle Button: Changes Rotation
          or Translation
      START: Rotation Manipulation Mode
          ("ROTATION" displayed on the screen)
      SELECT: Translate Manipulation Mode
          ("TRANSLATION" displayed on the screen)
      Directional Buttons: Change edit target (One with the cursor is
          selected target)
           Up/Down : changes models (MIMe1/MIM2/MIMe/MIMe4/ALL)
           Left/Right : Changes objects

<Remarks>
* How to use samples
1.After program starts, gradually press L/R buttons( L1,L2 R1, and/or R2)
  of Controller 1.
  When you press button(s), in the middle of the screen, you will see
  the outcome of the interpolation of the base and the MIMe-key(s)
  that corresponds to the button(s) you pressed. For detail of the Controller
  button allocation, refer to the section "Controller Button Assignment" of
  this document.

 Left:  Base
 Middle: Outcome of MIMe
 Right:  Each of the four L/R button corresponds to a MIMe key
             +---------------------------------+
             |                                 |
             |                       MIMe-key1 |
             |                                 |
             |Base Interpolated  MIMe-key2     |
             |Model    Model                   |
             |                       MIMe-key3 |
             |                                 |
             |                   MIMe-key4     |
             +---------------------------------+
                         Screen


2.Pressing either the Left or Right button of directional buttons
  (four on the left side) of Controller 1, the next preset model is displayed
  in the second line of upper left screen.

3.Pressing Start button of Controller 1 can toggle between the two joint-MIMes;
  axes-MIMe and RPY-MIMe.
  The MIMe currently selected is displayed in the first line of upper left
  screen.

4.You can edit models to create original ones using Controller 2.
  Select the target by directional button(s) first. Selected
  target will be displayed with a cursor and the current target ID is
  displayed in the second line on upper left corner of the screen.
  Pressing cross, circle, square or triangle button on the right hand
  side( hereafter refer to as Right four buttons) and R1 or R2 buttons
  can rotate/translate the selected model.
  Use Select button for rotation and Start button for translation.
  Left/Right directional buttons of Controller 1 reset to the preset state.

 - Articulated Model

     The model has the following hierarchical structure:
      (Name of parts inside of parentheses shows the origin)

    WORLD
     |
     +- torso (waist)
         |
         +- head (neck)
         |
         +- left thigh (crotch)
         |   |
         |   +- left leg  (knee)
         |
         +- right thigh (crotch)
         |   |
         |   +- right leg (knee)
         |
         +- left upper arm (shoulder)
         |   |
         |   +- left forearm (elbow)
         |
         +- right upper arm (shoulder)
             |
             +- right forearm (elbow)
