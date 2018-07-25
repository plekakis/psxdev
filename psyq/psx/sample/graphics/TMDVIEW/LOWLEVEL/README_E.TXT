$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

LOWLEVEL SMPLAE using GsDOBJ2

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.



tuto0
=====

<Description>

This program is simple object viewer using GsTMDfast...() which is low level
interface of packet making.

The difference of GsSortObject4() and GsTMDfast..() is the level of handler.
GsSortObject4() cope with OBJECT level and GsTMDfast copes with polygon type
level.

SortTMDFlat() is the substitution of GsSortObject4() , but only for F3 and TF3
type polygons. If you use GsTMDfast() interface you must decode the TMD format
by yourself. 


<How to Execute>

	> psymake
	> psymake load
	> run tuto0

<Controller Button Assignment>

<Controller Button Assignment>
        Up Directional Button   :	Transfer The Object(UP)
        Down Directional Button :	Transfer The Object(DOWN)
        Right Directional Button:	Transfer The Object(RIGHT)
        Left Directional Button :	Transfer The Object(LEFT)
        Square Button           :	Rotate The Object(LEFT)
        Cross Button            :	Rotate The Object(DOWN)
        Triangle Button         :	Rotate The Object(UP)
        Circle Button           :	Rotate The Object(RIGHT)
        R1 Button               :	Transfer The Object(NEARER)
        R2 Button               :	Transfer The Object(FARER)
        Select Button           :	Exit The Program

tuto1
=====
<Description>

(g4l.c, g4lfg.c, g4nl.c, tg4l.c, tg4lfg.c, tg4nl.c)

This program is a sample program using low level interface of packet making 
which written by DM(Direct Mapping) functions, without using GsTMDfast...().

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and
TG4 type polygons. 


<How to Execute>

	> psymake tuto1
	> psymake load2
	> run tuto1

<Controller Button Assignment>

	same as tuto0


tuto2
=====

<Description>

(g4trr.c, tg4trr.c)

This program is a sample program using low level interface of packet making
which written by DM functions. This program executes transfer, rotate and 
perspective in that order ,instead of RotTransPers().

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and 
TG4 type polygons.


<How to Execute>

	> psymake tuto2
	> psymake load2
	> run tuto2

<Controller Button Assignment>

	same as tuto0


tuto3
======
<Description>

(tg4lmip.c)

This program is a sample program using lowlevel interface of packet making
which supported mipmap (in tg4lmip.c).

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and
TG4 type polygons. This program uses low level interface of packet making
which written by DM(Direct Mapping) functions, without using GsTMDfast...().


<How to Execute>

	> psymake tuto3
	> psymake load3
	> run tuto3

<Controller Button Assignment>

	same as tuto0

