$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

scratch: How to use SCRACH PAD AREA

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

  This is a sample program showing the use of the scratchpad area(RAM in CPU).
  A simple memory access test is performed.

   (A) SCRACH PAD ACCESS 1 ( test0() )
       Access to the scratch pad.
       Indirect access using a pointer is performed.

   (B) SCRACH PAD ACCESS 2 ( test1() )
       Access to the scratch pad.
       Direct access using variable attributes is performed.

   (C) MAIN MEMORY ACCESS ( test2() )
       Access to standard main memory.

  The LOOP COUNT value becomes larger the faster the execution is.


<How to Execute>

  >psymake
  >run main


<Controller Button Assignment>

        Circle or Cross Button: The test mode is changed.


<Remarks>

  When this sample is compiled with the "-O0" (no optimization) option (see
  makefile.mak), the execution speeds of the programs are as follows 
  (going from fastest to slowest):
    (A) > (B) > (C)
  However, when compiling is done with the "-O3" option, the speeds are:
    (A) = (C) > (B) 
  Thus, it cannot always be said that the use of the scratch pad area
  necessarily leads to improvements in execution speed. This sample is
  just an example, and the question of which methods perform the best depends 
  on the specific code and degree of optimization.
