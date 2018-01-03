
PS-X Demonstration Program "janken"
==================================

* janken displays animation for a game of "paper, rocks and scissors" using modeling data for a hand that has a layered structure.


The following operations are also performed by the program:
Viewpoint movement
TOD data selection
Reading and displaying of TOD data for each frame.


* Execution
psymake
resetps 1
run \....\patchx.cpe
psymake load
run main

* Description of the files
makefile.mak        makefile

main.c              main routine
te.h                TMD ID table

tmd\te.tmd          modeling data

tod\appear.tod      TOD data (hand appears on the screen)
tod\jaanken.tod     TOD data (closed hand moves up and down)
tod\n_c.tod         TOD data (hand moves from starting position to show "scissors")
tod\c_n.tod         TOD data (hand moves from "scissors" to the starting position)
tod\n_g.tod         TOD data (hand moves from starting position to show "rock")
tod\g_n.tod         TOD data (hand moves from "rock" to starting position)
tod\n_p.tod         TOD data (hand moves from starting position to show "paper")
tod\p_n.tod         TOD data (hand moves from "paper" to starting position)
tod\disapp.tod      TOD data (hand exits by jumping off of the screen)

* The control pad operations are as follows:
arrow key (left):   viewpoint moves along the X axis in a positive direction
arrow key (right):  viewpoint moves along the X axis in a negative direction
arrow key (down):   viewpoint moves along the Y axis in a positive direction
arrow key (up):     viewpoint moves along the Y axis in a negative direction
L1 button:          viewpoint moves along the Z axis in a positive direction
L2 button:          viewpoint moves along the Z axis in a negative direction
Circle button:      plays the animation for "rock"
Triangle button:    plays the animation for "scissors"
Square button:      plays the animation for "paper"
X button:           randomly plays back either "rock", "paper", or "scissors"
Start button:       returns viewpoint to initial position
Select button:      exit



Copyright (C) 1994,1995 by Sony Computer Entertainment
All Rights Reserved
