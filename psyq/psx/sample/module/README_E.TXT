$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

module: Program reading and execution

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.
                                               Aug 15, 1997 Ver.1.4 yoshi

<Description>

There are 3 ways to read programs on a CD-ROM to execute.

   1. Executing the Exec() after reading EXE file with the CD function 
      or the Load()
   2. Using the LoadExec()
   3. Using the overlay

Each feature is described as follows. 
Refer to the following files for specific explanation, <How to Execute>
and <Controller Button Assignment>.

  PSX\SAMPLE\MODULE\EXECMENU\README_E.TXT
  PSX\SAMPLE\MODULE\OVERMENU\README_E.TXT


1) Executing the Exec() after reading EXE file with the CD function or 
   the Load()
  Advantage: The child program can start up by itself.
             This will be useful in developing multiple child programs
             in parallel.

  Disadvantage: Every interrupt must be stopped when the processing 
             switches between the parent and the child because the 
             libraries are linked to both the parent and the child.
             Memory efficiency is not good.

  Memory structure example:
  
      +---------------+ Initial value of stack pointer of parent
      |               |    The stack is consumed downward.
      |               |
      +---------------+ Initial value of stack pointer of child
      |               |    The stack is consumed downward.
      |               |
      |               |
      |               |
      +---------------+
      |     library   |
      |       --------+
      |               |
      | Child (.EXE)  |
      +---------------+ Child-loaded address
      |     library   |
      |       --------+ 
      |               |
      | Parent (.EXE) |
      +---------------+ Parent-loaded address
      | System area   |
      +---------------+

2) Using the LoadExec()
  Advantage: The child program can start up by itself.
             This will be useful in developing multiple child
             programs in parallel. The child process can be loaded
             over the parent, and in this case, both processes can use full
             2M bytes, and it is easier to develop. 

  Disadvantage: Every interrupt must be stopped when the processing 
             switches between the parent and the child because the 
             libraries are linked to both the parent and the child.
             The processing speed is slow because this function must be 
             called after _96_init() execution. 

Memory structure example:
( when the child is loaded over the parent.)

 
      +---------------+ Initial value of stack pointer 
      |               | 
      |               |
      |               |
      |               |
      |               |
      |               |
      |               |
      +---------------+
      |     Library   |
      |       --------+
      |               |
      | Parent (.EXE) |
      | Child (.EXE)  |
      +---------------+ Loaded address of parent and child
      | System area   | 
      +---------------+


3) Using the overlay
Advantage: The child process can be called by function.
            (Loading must be managed by the user.)
            The libraries are shared by the parent and the child, so
            the interrupts don't have to be stopped. Therefore, it is 
            possible to carry out other processing during loading.
            Memory efficiency is good.

Disadvantage: Some make-operations are needed, such as creating the 
            link-file, and setting the compile-options.
            Usually, since all libraries are linked to the parent, 
            the parent size will be larger.
            (If you want to link the library to the child, you should see
            PSYQ\PREFSMPL.)
           
    Memory structure example:

      +---------------+ Initial value of stack pointer
      |               |                ( shared by parent and child)
      |               |
      |               |
      |               |
      |               |
      |               |
      |               |
      +---------------+
      |               |
      | Child(.BIN)   |
      +---------------+ Child-loaded address
      |     Library   |
      |       (shared)|
      |       --------+
      |               |
      |               |
      | Parent (.EXE) |
      +---------------+ Parent-loaded address
      | System area   |
      +---------------+


								








