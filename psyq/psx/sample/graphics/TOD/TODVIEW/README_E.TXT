$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
todview: Sample Program of Animation with TOD Format Data

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<Description>

This is a most simple program which displays animation by using TOD
data. 

The following operations are also performed by the program:
-Viewpoint movement
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
Circle Button            : Plays the animation
Triangle Button          : Plays the animation
Square Button            : Plays the animation
Cross Button             : Plays the animation
Start Button             : Returns viewpoint to initial position
Select Button            : exit


<Remarks>

Description of the files

makefile.mak	: makefile

main.c		: main routine
te.h		: TMD ID table

te.tmd		: modeling data
jaanken.tod	: TOD data (closed hand moves up and down)

