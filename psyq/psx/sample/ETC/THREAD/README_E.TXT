$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
thread:Threads sample code

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

This sample program uses sample code/graphics/balls as a base to do the
following:

"after calculations are complete and a render command is issued, a thread is
used to perform a different operation until the next Vsync interrupt is
received."

Changes are marked by:

#ifdef THREAD_sample code
(#else)
#endif

main.c is compiled with THREAD_sample code defined. The time spent on the sub
thread (the number of loops that were performed before the next idle
time/Vsync was received) is displayed on the screen instead of the processing
time. The processing time for the main loop increases and the "idle time"
decreases when the number of displayed balls is increased. When using the sub
thread with the SUB_STACK, the code must be changed since the stack pointer is
defined.

The operations relating to the sub thread are as follows.

1* Once a render command is issued after calculations are complete, ChangeTh()
is used to transfer control to the sub thread (function is sub_func()).
Processing resumes from where the previous Vsync interrupt was received.
2* A counter is incremented in a loop until the next Vsync interrupt is
received.
3* When the Vsync interrupt is received, the function specified by
VsyncCallback (cbvsync) is executed. When this function is exited, processing
is resumed in main thread.
4* When interrupt operations are done, processing resumes from the step after
ChangeTh() in 1.

The diagram below illustrates what takes place.


           main thread           *    sub thread
       ***************************************************
                                 *
        main()                   *
        {                        *
  initialization* pre-processing *
                |                *
          -> (MAIN LOOP)         *                       
          |     |                *    sub_func()
          | Calculations* Rendering  *    {
          |     |                *    +----------------+ 
          | ChangeTh() -------------->| Counter Upper Loop|
          |     | <-----------------  |(From previous operation)|
          | Swap display buffers * |  +-----/----------+
          |     |                * |  }    /
          |------                * |      /Vsync interrupt
                |                * |     / 
        post-processing          * |   cbvsync()
                |                * |   {   
            return(0)            * |     FntPrint(....)
        }                        * |     Change return thread
                                 * |---}


The following is a comparison of the advantages and disadvantages of the use
of ChangeTh() compared to the use of function calls to perform the same
operations.

* Advantages * 
When ChangeTh() is used, processing can pick up from the point where the
interrupt was received. This makes it easier to write the code. When the same
operations are performed using function calls, the code becomes complicated.
Functions require exit operations from the function when the interrupt
completes (in general, the interrupt operation sets a flag and this flag is
checked at the beginning of the loop). The function must also provide resume
operations since the next function call begins from the start of the function
(the returning point and partial results of the interrupted function must be
saved and processed).

* Disadvantages *
When ChangeTh() is used, context switching takes place. This imposes a
somewhat heavier load compared to the use of function calls (although most of
this simply involves the number of registers in which values are saved). Also,
since a sub thread is used, a separate stack must be prepared and this area
must be kept intact until the point when the function is resumed. Additional memory management is required compared to function calls, where information is simply put on the main stack.

* Other *
If a single function will be executed simultaneously as multiple threads,
the code must be reentrant. Please note that a number of PlayStation library functions do not support this.

<How to Execute>
	> psymake 	compile
	> run balls	run

<Controller Button Assignment>
        Up Directional Button   :	increase of balls
        Down Directional Button :	decrease of balls
        L1 Button               :	pause
        Select Button           :	quit

<Remarks>
