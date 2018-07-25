$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

 			SAMPLE PROGRAM FOR TransRot

 This program is a sample of the TransRot... function, which eliminates gaps
 generated when adjacent polygons are in different coordinate systems.
 In this case, nine cubes are placed in coordinate systems that are offset by the
 length of the side of a cube. When the overall coordinate system (world) is not
 rotating, the coordinates of adjacent vertices match up so that there are no
 gaps between the cubes. However, if the world is rotated, RotTransPers will add
 the rotated translation separately. The margin of error in
 calculations can lead to vertices not matching up and gaps being created.
 In the TransRot... function, translation is added first, and then
 rotation is applied, thus preventing gaps from being generated.


 	PADLup button... gaps created by RotTransPers 
 	PADLleft button... no gaps from TransRotPers
 	PADLdown button... no gaps from TransRotPers3
 	PADLright button... no gaps from TransRot_32(can calculate 
						up to a 32bit space)

