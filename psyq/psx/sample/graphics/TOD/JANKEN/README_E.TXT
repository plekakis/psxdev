$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
janken: Sample Program of Animation with TOD Format Data

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<Description>

This is a sample program which displays animation by using more than
one TOD data.  This program, "janken", displays the animation for a
game of "paper, rock and scissors", using modeling data for a hand
that has a layered structure.

The following operations are also performed by the program:
-Viewpoint movement
-TOD data selection
-Reading and displaying of TOD data for each frame


<How to Execute>

>psymake	compile
>psymake load	load data	
>run main	execution


<Controller Button Assignment>

Left Directional Button  : Viewpoint moves along the X axis in a positive direction
Right Directional Button : Viewpoint moves along the X axis in a negative direction
Down Directional Button  : Viewpoint moves along the Y axis in a positive direction
Up Directional Button    : Viewpoint moves along the Y axis in a negative direction
L1 Button                : Viewpoint moves along the Z axis in a positive direction
L2 Button                : Viewpoint moves along the Z axis in a negative direction
Circle Button            : Plays the animation for "rock"
Triangle Button          : Plays the animation for "scissors"
Square Button            : Plays the animation for "paper"
Cross Button             : Randomly plays back either "rock", "paper", or "scissors"
Start Button             : Returns viewpoint to initial position
Select Button            : exit


<Remarks>

Description of the files

makefile.mak	: makefile

main.c		: main routine
te.h		: TMD ID table

tmd\te.tmd	: modeling data

tod\appear.tod	: TOD data (hand appears on the screen)
tod\jaanken.tod	: TOD data (closed hand moves up and down)
tod\n_c.tod	: TOD data (hand moves from starting position to show "scissors")
tod\c_n.tod	: TOD data (hand moves from "scissors" to the starting position)
tod\n_g.tod	: TOD data (hand moves from starting position to show "rock")
tod\g_n.tod	: TOD data (hand moves from "rock" to starting position)
tod\n_p.tod	: TOD data (hand moves from starting position to show "paper")
tod\p_n.tod	: TOD data (hand moves from "paper" to starting position)
tod\disapp.tod	: TOD data (hand exits by jumping off of the screen)
