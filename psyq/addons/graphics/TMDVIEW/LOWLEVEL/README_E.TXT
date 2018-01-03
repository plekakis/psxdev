$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$


tuto0.c

This program is simple object viewer using GsTMDfast...() which is low level
interface of packet making.

The difference of GsSortObject4() and GsTMDfast..() is the level of handler.
GsSortObject4() cope with OBJECT level and GsTMDfast copes with polygon type
level.

SortTMDFlat() is the substitution of GsSortObject4() , but only for F3 and TF3
type polygons. If you use GsTMDfast() interface you must decode the TMD format
by yourself. 


tuto1.c 
(g4l.c, g4lfg.c, g4nl.c, tg4l.c, tg4lfg.c, tg4nl.c)

This program is a sample program using low level interface of packet making 
which written by DM(Direct Mapping) functions, without using GsTMDfast...().

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and
TG4 type polygons. 


tuto2.c
(g4trr.c, tg4trr.c)

This program is a sample program using low level interface of packet making
which written by DM functions. This program executes transfer, rotate and 
perspective in that order ,instead of RotTransPers().

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and 
TG4 type polygons.

tuto3.c
(tg4lmip.c)

This program is a sample program using lowlevel interface of packet making
which supported mipmap (in tg4lmip.c).

You need DMPSX to make this sample program.

SortTMDobject() is the substitution of GsSortObject4(), but only for G4 and
TG4 type polygons. This program uses low level interface of packet making
which written by DM(Direct Mapping) functions, without using GsTMDfast...().
