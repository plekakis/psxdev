$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
bgsample: Sample Program of background screen

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<Description>

This is a sample program which displays background screen by using
data created by sprite editor.


<How to Execute>

>psymake	compile
>psymake load	load data	
>run main	execution


<Controller Button Assignment>

Left Directional Button  : Scroll to the left
Right Directional Button : Scroll to the right
Down Directional Button  : Scroll down
Up Directional Button    : Scroll up
L1 Button                : Left rotation
L2 Button                : Right rotation
Circle Button            : Narrow width of background
Square Button            : Widen width of background
Cross Button             : Grow height of background
Triangle Button          : Decrease height of background
Select Button            : exit


<Remarks>

Description of the files

makefile.mak	: makefile

main.c		: main routine

sample.bgd	: BG map data
sample.cel	: BG cell data
sample.tim	: texture data
