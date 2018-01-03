$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

PS-X Demonstration Program "rcube"
==================================

* rcube is a program that displays 44 freely rotating colored cubes.
Multiple GsDOBJ2s are used and are moved independently.

The following operations are performed in this program:
Point of view movement
TMD data conversion
Fog setting

* Execution
psymake
resetps
runcube


* Description of files
makefile.mak        makefile
runcube.bat         batch file for execution

main.c              main routine
table.h             table for performing rotations
pos.h               defines the initial position of each cube

rcube.tmd           modeling data (contains 4 models)
rcube.tim           texture data

* Control pad operations are as follows:

arrow keys:         rotate cube group

circle button:      reduce (move away from cube)
X button:           enlarge (move toward cube)

square button:      FOG ON/OFF
triangle button:    select material (normal->semi-transparent->Gouraud->texture)

L1 button:          explode
R1 button:          reset explosion

SELECT button:      exit



Copyright (C) 1994,1995 by Sony Computer Entertainment
All Rights Reserved
