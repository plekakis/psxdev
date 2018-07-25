-------------------
FILE:	Sampler.zip

------------------
DATE:	September 17, 1997

--------------------
CONTENTS OVERVIEW:
This zip file contains the specifications for
the Sony Interactive Sampler CD, which is used
for marketing promotion of titles in the North 
American territories.  

It is based in large part on the Demodisc material in 
\psx\sample\scee\Demodisc, but is different.

----------------------------------------------------
INSTRUCTIONS:
Unzip the file using Pkunzip or Winzip into
a suitable directory.  Then read the "demospec.doc" or "demospec.pdf"
for detailed information on the specifications 
required of your demonstration programs.

-----------------------------------------------------
CONTENTS IN DETAIL:
There are 5 directories that include the various files you will need to construct
a demonstration of your program for use in a Sony Interactive Sampler. 
It is intentionally barebones for developers to paste in their own creations.

 
  \launcher:    This is a simple test harness, which will repeatedly load and
                launch your demonstration. Included is source and a readme. 

  \menu:        This is a simple menu which list the possible child programs to
                run.  This program also shows how to retrieve arguments from the 
                launcher, and a known to be functional way to set up and 
                close down the various subsystems

  \startup:     This directory contains the source and .obj file that replaces
                the startup code in libsn.lib, allowing your program to accept 
                arguments, and to return control to the launcher.
                This version includes the libsn variables __heapbase et al.

   \setsp:      This directory contains 2 PC utilities for working with
                PlayStation .EXE files - dumpexe, which dumps the information
                from the XF_HDR info at the start of an EXE file, and setsp,
                which can be used to set the stack pointer in a PlayStation 
                EXE file.

   \example:    This directory has two example programs which runs under 
                launch.exe.

All of this specification, from launcher source to the documentation, is based
largely on work by the guys at SCEE.

If you have problems or questions, contact:
Sony Computer Entertainment
Technical Support
(650) 655 8181 

or via email:  devtech_support@playstation.sony.com

----------------------------------------------------
Copyright (C) 1997-1999. Sony Computer Entertainment Inc.