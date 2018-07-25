$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

PS-X Demonstration Program "rcube"

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	rcube is a program that displays 44 freely rotating colored cubes.
	Multiple GsDOBJ2s are used and are moved independently.

	The following operations are performed in this program:
	Point of view movement
	TMD data conversion
	Fog setting

	
<How to Execute>

	>psymake
	>resetps
	>runcube

<Controller Button Assignment>

        Up Directional Button   :	rotate cube group(UP)
        Down Directional Button :	rotate cube group(DOWN)
        Right Directional Button:	rotate cube group(RIGHT)
        Left Directional Button :	rotate cube group(LEFT)
        Square Button           :	FOG ON/OFF
        Cross Button            :	enlarge (move toward cube)
        Triangle Button         :	select material(normal->
					semi-transparent->Gouraud->texture)
        Circle Button           :	reduce (move away from cube)
        L1 Button               :	explode
        R1 Button               :	reset explosion
        Select Button           :	Exit The Program

<Remaks>

[Description of files]

makefile.mak        makefile
runcube.bat         batch file for execution

main.c              main routine
table.h             table for performing rotations
pos.h               defines the initial position of each cube

rcube.tmd           modeling data (contains 4 models)
rcube.tim           texture data

