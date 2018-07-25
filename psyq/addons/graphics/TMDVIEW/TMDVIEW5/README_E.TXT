$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

TMDVIEWÇT	tmdviewer using GsDOBJ5


				1994.12.9
				CopyRight (C) Sony Computer Entertainment

tuto0:	simple tmdviewer

Åyhow to runÅz
		make tuto0	compile & load files

Åykey assignÅz

	left direction buttons		transfer XY
	right direction buttons		rotation
	right upper buttons		transfer Z

Åymodeling dataÅz

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

Åytexture dataÅz
		pat4.tim		4bit clut texture data
		pat8.tim		8bit clut texture data
		pat16.tim		16bit direct texture data
		pat4t.tim		4bit clut transparent texture data
		pat8t.tim		8bit clut transparent texture data
		pat16t.tim		16bit direct transparent texture data

ÅycommentsÅz
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


tuto1:	sample code for split screen using GsDOBJ5

Åyhow to runÅz
		make tuto1	compile & load files

Åykey assignÅz
		same as tuto0

Åymodeling dataÅz

		same as tuto0
ÅycommentsÅz
		This program split screen upper and lower part.
		The viewpoint of each screen is different.

		The modeling data is common with upper and lower screen,
		but object hadlers are different.

		GPU drawing command is quing by libgpu so tehre is no
		wainting sequence in this program.


tuto2: sample code for multi ot and using same object with different hadlers.

Åyhow to runÅz
		make tuto2	compile & load files

Åykey assignÅz

		right direction key	rotate object0
		right upper key		zoom in / out object0
		left direction key	rotate object1
		left upper key		zoom in / out object1


Åymodeling dataÅz

		same as tuto0

ÅycommentsÅz
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



tuto3:	sample code for the runtime dividing.

Åyhow to runÅz
		make tuto3	compile

Åykey assignÅz

		same as tuto0

Åymodeling dataÅz

		same as tuto0

ÅycommentsÅz
		This program is dividing the selected object at runtime.

		To select dividing is set the GsDIV1(2x2 4) or GsDIV2(4x4 16)
		or GsDIV3(8x8 64) or GsDIV4(16x16 256) in the attribute field
		of the object handler.

		If you set the divide attribute , you must prepare the
		packet area to make packet for divided polygons by calling
		GsSetWorkBase().


tuto4:	multi screen coordinate sample

Åyhow to makeÅz
		psymake tuto4	compile

Åykey assignÅz

		left arrow keys		XY transfer 
		right arrow keys	rotation

		right upper key far	zoom out
		right upper key near	zoom in
		left upper key far	screen coordinate normal scale
		left upper key near	screen coordinate 1/8 scale

Åymodeling dataÅz

		same as tuto0

ÅycommentsÅz
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


tuto5:	jump table sample code

Åyhow to makeÅz
		psymake tuto5	compile

Åykey assignÅz
		same as tuto0

Åymodeling dataÅz

		ONLY cube3.tmd

ÅycommentsÅz
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

tuto6:	sample code of subjective move.

Åyhot to runÅz
		psymake tuto6	compile & load files

Åykey assignÅz
		right arrow keys	the rotation of viewpoint
		left arrow keys		the transfer of viewpoint
		left upper key		the transfer along Z of viewpoint
		right upper key		the rotation along Z of viewpoint

Åymodeling dataÅz

		same as tuto0

ÅycommentsÅz
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


								à»è„
