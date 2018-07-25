$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
fballs   sample program for decreasing the drawing time

                     Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.


<Descriptions>
	This is a sample program based on "balls" program for decreasing 
	the drawing time.

	Changes from the original balls program are as follows:

	o The display mode was changed to 640x480.

	o The sprite primitives of balls were changed to semi-transparency.

	And the following 3 points were changed for decreasing the drawing 
	time.

	o To decrease the time of V-BLNK, 242 was put into h member of
	  struct screen of struct DISPENV.
	  By changing the display position between V-RAM and Display,
	  the time of V-BLNK could be decreased.

	o The amount of drawing to V-RAM was decreased by changing
	  the drawing heignt to 464.
	  By this change, the clipping area could be large and the amount
	  of drawing to V-RAM would be decreased.

	o By using DrawOTagEnv() to do PutDrawEnv and DrawOTag at the same
	  time, the overhead of interruption was decreased.

	By these changes, the drawing time could be decreased a little as 
	follows.

			a number of balls
	Normal		2021
	Fast		2085	

<How to execute>
        psymake         compile
        run balls       run

<Controller Button Assignment>
        Up/Down Directional Button  : decrease/increase the number of balls
        L1 Buttons   	            : pause
        Select Button     	    : quit
