$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

TMDVIEW5	tmdviewer using GsDOBJ5


                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

tuto0:	simple tmdviewer
======

<Description>

	GsDOBJ5 is the handler of preset TMD format data.
	It is done realtime to preset TMD format data , so
	you don't have to prepare another TMD format.

	Preset means making packets of all polygons in the TMD data,
	and write the value of packets which is changed.
	In this way, it makes faster by reducing memory accesses.
	For example , it dosen't change the RGB value in the packet
	in the case of not shading.

	It is important to grouping polygons by it's type for
	presetting. TMDSORT.EXE is the tool for grouping.

	GsSortObject5() is two times faster than GsSortObject4()
	at the peek.

	Auto-divide means divide all polygons of the object
	into the fix number according to the attribute.

	If you set the divide attribute , you must set the
	packet address area by GsSetWorkbase().


<How to Execute>

	>psymake tuto0
	>psymake load
	>run tuto0

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

<Remarks>

[modeling data]

	There are 16 modeling data.
	The type of modeling data is categolized by extension of the
	file name.

		g :	gour shading model
		t :	transparent model
		l :	preshaded model

	The shape of modeling data is cube, three surface are
	non textured and remaining three surfaces are 2/8/15 bit
	texture mapped.
		
	You can see all modeling data by this program,loading
	each data.


	cube3.tmd		flat polygon
	cube3g.tmd		gour polygon
	cube3gl.tmd		gour preshaded polygon
	cube3gt.tmd		gour transparent polygon
	cube3gtl.tmd		gour preshaded transparent polygon
	cube3l.tmd		flat preshaded polygon
	cube3t.tmd		flat transparent polygon
	cube3tl.tmd		flat preshaded transparent polygon

	cube4.tmd		flat quad polygon
	cube4g.tmd		gour quad polygon
	cube4gl.tmd		gour preshaded quad polygon
	cube4gt.tmd		gour transparent quad polygon
	cube4gtl.tmd		gour preshaded transparent quad polygon
	cube4l.tmd		flat preshaded quad polygon
	cube4t.tmd		flat transparent quad polygon
	cube4tl.tmd		flat preshaded quad transparent polygon

	and sample modeling data shuttle1.tmd

[texture data]

	pat4.tim		4bit clut texture data
	pat8.tim		8bit clut texture data
	pat16.tim		16bit direct texture data
	pat4t.tim		4bit clut transparent texture data
	pat8t.tim		8bit clut transparent texture data
	pat16t.tim		16bit direct transparent texture data


tuto1:	sample code for split screen using GsDOBJ5
=====

<Description>

	This program split screen upper and lower part.
	The viewpoint of each screen is different.

	The modeling data is common with upper and lower screen,
	but object hadlers are different.

	GPU drawing command is quing by libgpu so tehre is no
	wainting sequence in this program.


<How to Execute>

	>psymake tuto1
	>psymake load
	>run tuto1

<Controller Button Assignment>

	same as tuto0

<Remarks>

[modeling data]

		same as tuto0

tuto2: sample code for multi ot and using same object with different hadlers.
======
<Description>

	This program is sample code for same object with
	diffrent handlers, and sorting multi OTs.

	"point" of Wot2 which is set by GsClearOt(offset,point,Wot2)
	is the reference point of Wot1.
	So GsSortOt(&Wot2,&Wot1) sorts Wot2 to Wot1 by the "point"
	value of Wot2.

	Because "point" value is 0 in the sample code, object2
	sorted to Wot2 is always front of the object sorted
	Wot1.
	If you set the point value 1<<OT_LENGTH-1,object2 is
	always back of object1.

<How to Execute>

	>psymake tuto2
	>psymake load
	>run tuto2

<Controller Button Assignment>

        Up Directional Button   :	Rotate The Object1(UP)
        Down Directional Button :	Rotate The Object1(DOWN)
        Right Directional Button:	Rotate The Object1(RIGHT)
        Left Directional Button :	Rotate The Object1(LEFT)
        Square Button           :	Rotate The Object0(LEFT)
        Cross Button            :	Rotate The Object0(DOWN)
        Triangle Button         :	Rotate The Object0(UP)
        Circle Button           :	Rotate The Object0(RIGHT)
        L1 Button               :	Transfer The Object1(NEARER)
        L2 Button               :	Transfer The Object1(FARER)
        R1 Button               :	Transfer The Object0(NEARER)
        R2 Button               :	Transfer The Object0(FARER)
        Select Button           :	Exit The Program


<Remarks>

[modeling data]

		same as tuto0


tuto3:	sample code for the runtime dividing.
======

<Description>

	This program is dividing the selected object at runtime.

	To select dividing is set the GsDIV1(2x2 4) or GsDIV2(4x4 16)
	or GsDIV3(8x8 64) or GsDIV4(16x16 256) in the attribute field
	of the object handler.

	If you set the divide attribute , you must prepare the
	packet area to make packet for divided polygons by calling
	GsSetWorkBase().

<How to Execute>

	>psymake tuto3
	>psymake load
	>run tuto3

<Controller Button Assignment>

	same as tuto0

<Remarks>

[modeling data]

		same as tuto0


tuto4:	multi screen coordinate sample
======

<Description>

	This program is sample code for scaling the SCREEN coordinate
	by calling GsScaleScreen().

	If you scale the SCREEN coordinate, unfortunately the distance
	view point and object excess 16 bit , the object is not
	clipped.

	For example in this sample code , the distance the object and
	the viewpoint excess 16	bit , but this program scales 1/8
	the SCREEN coordinate so it is possible for displaying
	within 19 bit.

	Notice that if you scale the SCREEN coordinate , OT's scale
	is changed according to the SCREEN scale.
	It must not do that different scaling objects aresorted to 
	the OT by the same scaling.

	For ecample if you manage to sort the 1/8 scale screen object
	to the OT, you must add 3 to the shift value on calling
	SortObject... 


<How to Execute>

	>psymake tuto4
	>psymake load
	>run tuto4

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
        L1 Button               :	screen coordinate normal scale
        L2 Button               :	screen coordinate 1/8 scale
        Select Button           :	Exit The Program

<Remarks>

[modeling data]

		same as tuto0


tuto5:	jump table sample code
======

<Description>

	tuto5 is the same function as tuto0 . The only difference is
	GsSortObject5J() ( tuto5.c ) and GsSortObject5() ( tuto0 ).
	If you call GsSortObject5J() in sted of GsSortObject5() ,
	you minimize the object code by eliminating sub functions
	called in GsSortObject5().
		
	The difference tuto0.c and tuto5.c are calling "jt_init()" in
	"init_all()" and GsSortObject5J() insted of GsSortObject5().

	Because I register only GsPrstF3L and GsPrstTF3L in
	"jt_init()", this program only cope with textre and non-texture
	flat triangle polygons ,tt is not possible for dividing ,
	FOG ,lighting off.

	The code size is 48 kbyte less than tuto0 because of
	eliminating sub functions.

	If you want more kinds of polygons , you register more
	sub functions to the jump table.
	If you register all sub functions GsSortObject5J() equals
	to GsSortObject5().
	Scalton of register sub functions is listed "jt_init5()"
	in libgs.h.

	If you want to know which sub functions to register ,
	you register prefix "dmy" sub functions.(ex. dmyGsPrstF3L,
	dmyGsPrstTF3L...) Thease functions print it's name on the
	first call.

<How to Execute>

	>psymake tuto5
	>psymake load5
	>run tuto5

<Controller Button Assignment>

	same as tuto0

<Remarks>
[modeling data]

		ONLY cube3.tmd	



tuto6:	sample code of subjective move.
======

<Description>

	This program is sample code for moving subjectivly of
	the viewpoint.
	The all moves assigned to the kes are oriented by
	the subjective coordinate. For example transfer along Z
	means transfer along viewing Z.

	This program prepare the coordinate named DView and
	set the camera at that coordinate.
	The transfer of the DView must be subjective , so
	tarnslate subjective transfer to WORLD tarnsfer	by
	multiply Divew.coord.

	It is complicated to solve the rotate matrix evry frame
	by it's orign point for the subjective rotation, so
	this program use the algorizm of solving the rotate
	matrix by the previous frame. But this method cotains
	some errors. If you want to avoid this error , you must
	call MatrixNormal(). This fuction translate input matrix
	to regular matrix.

	Note that transfer elements have some errors after calling
	MatrixNormal().

<How to Execute>

	>psymake tuto6
	>psymake load
	>run tuto6

<Controller Button Assignment>

        Up Directional Button   :	Transfer The ViewPoint (UP)
        Down Directional Button :	Transfer The ViewPoint (DOWN)
        Right Directional Button:	Transfer The ViewPoint (RIGHT)
        Left Directional Button :	Transfer The ViewPoint (LEFT)
        Square Button           :	Rotate The ViewPoint (LEFT)
        Cross Button            :	Rotate The ViewPoint (DOWN)
        Triangle Button         :	Rotate The ViewPoint (UP)
        Circle Button           :	Rotate The ViewPoint (RIGHT)
        L1 Button               :Rotate The ViewPoint (clockwize around Z)
        L2 Button               :Rotate The ViewPoint (unclokwize around Z)
        R1 Button               :	Transfer The ViewPoint (Nearer)
        R2 Button               :	Transfer The ViewPint (Fraer)
        Select Button           :	Exit The Program


<Remarks>
[modeling data]

		same as tuto0

