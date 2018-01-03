$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       
mipmap : sample program using mip-map

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	This is a sample program using mip-map. 

	To use the mip-map, GsSortObject4J() has to be used and 
	GsTMDfastTF4LM() has to be registered to the jump table.

	This function GsTMDfastTF4LM executes mip-map that changes a
	texture corresponding the size of polygon for the Flat-texture 
	quadrangles in the TMD data.

	The texture patterns should be located on the V-RAM as follows.

                +--------------+-------+
                |              |       |
                |              |       |
                |              |       |
                |              +---+---+
                |              | +-+   |
                |              | +-+---+
                |              |
                +--------------+

	The size of textures are 1, 1/4, 1/16 and 1/256 of the original one.
	The outerproduct are used for which texture is used.

	And also, the order of the top of polygons should be as follows.

                0        1
                +--------+
                |        |
                |        |
                |        |
                |        |
                +--------+
                2        3
	

<How to Execute>
	>psymake		compile
	>psymake load		load data
	>run tuto0		run

<Controller Button Assignment>
        Directional Button      : rotation of the object
        Square/Circle Button    : rotation of the object
        L1 Button               : reset the rotation
        Start Button            : switch mip-map and original
        Select Button           : quit

<Remarks>
