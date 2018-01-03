$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

TMDVIEW3	PMD View using GsDOBJ3


				1994.12.9
				CopyRight (C) Sony Computer Entertainment

tuto0:	the most simple PMD viewer

[how to build]
		make tuto0	compile and load

[key assign]
	left direction buttons 		transfer XY
	right direction buttons 	rotation
	r1/r2 button			transfer Z

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


[explanation]

		GsDOBJ3 is the object handler for PMD format modeling data.
		GsSortObject3() have the function of perspective transformation
		and Z sorting using OT for PMD format modeling data.
		GsLinkObject3() links PMD data and the object handler.

