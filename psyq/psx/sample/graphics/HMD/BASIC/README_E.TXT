$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

BASIC	viewer using HMD format


			     Copyright(C) 1997 Sony Computer Entertainment Inc.
			     				   All rights reserved.


tuto0: general HMD viewer

<explanation>
	This sample is a general viewer using HMD format.
	This includes all primitive drivers which are supported by libgs
	currently.

<how to run>
	psymake tuto0.cpe	compile
	psymake load0		load data
	run tuto0		run

<key assign>
	right direction buttons	:	rotation of the object
	left direction buttons	:	transfer XY of the object
	L1/R1 buttons		:	transfer Z of the object
	L2/R2 buttons		:	change a COORDINATE number which is 
					affected

	START			:	change a COORDINATE number which is 
					affected to 0
	SELECT			:	change a COORDINATE number which is 
					affected to number of COORDINATE - 1

	START & SELECT		:	quit

<modeling data>
	tri.hmd		:	Triceratops
	dino.hmd	:	dinosaurs(Tyrannosaurus)
	cube.hmd	:	cube
	tile.hmd	:	cube with texture pattern repeat
	ground.hmd	:	ground


tuto1: HMD viewer using shared polygons

<explanation>
	This sample is a viewer using shared polygons of HMD format.
	Shared polygon means a polygon which has the plural number of
	matrix.
	
	By using this shared polygons, one-skin modeling could be possible.

<how to run>
	psymake tuto1.cpe	compile
	psymake load1		load data
	run tuto1		run

<key assign>
	right direction buttons	:	rotation of the whole object
	left direction buttons	:	transfer XY of the whole object
	L1/R1 buttons		:	transfer Z of the whole object

	with pressing start button
	right direction buttons	:	rotation of object 1
	left direction buttons	:	transfer XY of object 1
	L1/R1 buttons		:	transfer Z of object 1

	with pressing select button
	right direction buttons	:	rotation of object 2
	left direction buttons	:	transfer XY of object 2
	L1/R1 buttons		:	transfer Z of object 2

	START & SELECT		:	quit

<modeling data>
	flag.hmd	:	French national flag


tuto2: shuttle viewer using HMD format

<explanation>
	This sample is a viewer using hierarchy of HMD format.

	As HMD format can have matrices and a parent-chile relationship 
	in the data, program and data are used independently.

	In this data, copying objects(hatch and wings of satellite) are used, 
	copying say same modeling data but different model.

<how to run>
	psymake tuto2.cpe	compile
	psymake load2		load data
	run tuto2		run

<key assign>
	right direction buttons	:	rotation of the object
	left direction buttons	:	transfer XY of the object
	START/SELECT buttons	:	transfer Z of the object
	L1/R1 buttons		:	open/close the hatch
	L2/R2 buttons		:	up/down the satellite

	START & SELECT		:	quit

<modeling data>
	shuttle.hmd	:	shuttle


tuto3: HMD viewer using sub divide

<explanation>
	This sample is a viewer using sub divide of HMD format.
	The sub divide of HMD is performed by changing primitive drivers.
	And the number of sub divide must be put to upper 8 bits of primitive
	index.

	+-------------------------------+-------------------------------+
	|                              TYPE                             |
	+-------------------------------+-------------------------------+
	|     number of polygons        |              size             |
	+---------------+---------------+-------------------------------+
	|no. of sub div |                    PRIM IDX                   |
	+---------------+-----------------------------------------------+

	There are Macros to specify the number of sub divide
	(GsUNIT_DIV1-GsUNIT_DIV5) in libgs.h.

	And also, in the case of the Active sub-divide, Z value and scale of
	polygon need to be set by using GsSetAzwh().

<how to run>
	psymake tuto3.cpe	compile
	psymake load3		load data
	run tuto3		run

<key assign>
	right direction buttons	:	rotation of the object
	left direction buttons	:	transfer XY of the object
	L1/R1 buttons		:	transfer Z of the object
	L2/R2 buttons		:	sub divide mode switch

	START			:	Line display on
	SELECT			:	Line display off

	START & SELECT		:	quit

<modeling data>
	jimen.hmd	:	ground


normview: HMD normal vector viewer

<explanation>
	This sample is a viewer to see normal vectors using HMD format.
	In case of a flat polygon, a red line is drawn from the center
	of the polygon to the direction of the normal vector, and in case 
	of a gouraud polygon, from the vertices to the normal vector.

	To draw the normal vector, the sample primitive drivers in 
	../common  are used.
	The following sample primitive drivers are ready to use.

		GsU_f0000008:	flat triangle
		GsU_f0000009:	flat textured triangle
		GsU_f000000c:	gouraud triangle
		GsU_f000000d:	gouraud textured triangle
		GsU_f0000010:	flat quadrangle
		GsU_f0000011:	flat textured quadrangle
		GsU_f0000014:	gouraud quadrangle
		GsU_f0000015:	gouraud textured quadrangle

<how to run>
	psymake normview.cpe	compile
	psymake load0		load data
	run tuto0		run

<key assign>
	right direction buttons	:	rotation of the object
	left direction buttons	:	transfer XY of the object
	L1/R1 buttons		:	transfer Z of the object
	L2/R2 buttons		:	change a COORDINATE number which is 
					affected

	START			:	change a COORDINATE number which is 
					affected to 0
	SELECT			:	change a COORDINATE number which is 
					affected to number of COORDINATE - 1

	START & SELECT		:	quit


<remark>
	The model data "tri.hmd" is made from the data created by Viewpoint
	DataLabs and distributed by Newtek Incorporated.
