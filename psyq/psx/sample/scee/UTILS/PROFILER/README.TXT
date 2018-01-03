PlayStation Profiler
====================

Copyright (c) Sony Computer Entertainment 1994-1995.  All rights reserved.

This code allows you to profile your programs for CPU and GPU use.
Compile up the example (you'll need to change the LNK file to allow linking with
your libraries) and you'll see two bar graphs at the top of the screen.

The top bar graph shows CPU usage, the bottom GPU drawing speed.  The longer the
green/red bars are, the more CPU/GPU time is being used.  The scale of the axes is
set (in protest.c) by ProfileInit(1) - this means that one marking on the grid 
represents one frame.  In other words, if the GPU bar reaches half way along the
screen, then it is drawing for the whole frame.  If the bar is always below this point,
the program will run at 60fps.  If the bar is between half way and all the way along the
screen, the program is running at 30fps etc.

The test program basically draws a number of POLY_G3s (all on top of one another).  
The number of polys drawn is varied with the up and down buttons on the right hand of 
the pad (you can move the viewpoint around by pressing other buttons).  Notice how 
both the CPU and GPU bars change.

Once you have called ProfileInit (which you only need to do once), you should call 
ProfileAddDrawOT() after DrawSync.  This draws the GPU time bar.  If you wish to use the 
CPU timer, you must call ProfileReadCount, then ProfileAddOT().  


For the CPU graph, the time represented is the time between the ProfileStartCount and 
the ProfileReadCount (ie the first ProfileReadCount is meaningless in the example).

For GPU, the time is measured between the ProfileStartCount and the end of GPU drawing 
(DrawSyncCallback).


