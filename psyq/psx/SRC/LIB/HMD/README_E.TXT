$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                   Copyright (C) 1998 Sony Computer Entertainment Inc.
                                                  All Rights Reserved.

Disclosure of the HMD Primitive Driver Source

The source code for the HMD primitive driver is disclosed in Runtime 
Library Release 4.4.

Purpose:
	- To provide the user with reference in tuning when a game is 
	  created. 
	- To enable the user to obtain the full benefits the HMD 
	  architecture can provide.
	- To provide the user with reference when the user's own HMD 
	  primitive driver is created.

Method:
	- All the primitive driver source codes are disclosed, 
	  including those written in assembler.
	- The assembler source can be assembled with ccpsx.
	  Please note that it cannot be assembled with macro assembler 
	  asmpsx.
	- Individual source codes are completely the same as those used 
	  in SCEI to create libhmd.
	- The framework functions other than the primitive driver are 
	  not disclosed.
	- The source code is stored in \psx\src\lib\hmd\ directory.

Note:
	- All the copyrights belong to Sony Computer Entertainment Inc.
	- When creating a game, the offered source code can freely be 
	  modified and built into the game.   
	- The actions to be taken against the bugs and failures in the 
          source code are equivalent to those against ordinary libraries.
