$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR TransRot

                     Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                   All rights reserved.

<Descriptions>
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

<How to execute>
	psymake		compile
	run main	run

<Controller Button Assignment>
        Up Directional Button   :gaps created by RotTransPers
        Down Directional Button :no gaps from TransRotPers3
        Right Directional Button:no gaps from TransRot_32(can calculate
					up to a 32bit space)
        Left Directional Button :no gaps from TransRotPers
        Square Button           :rotation of the object
        Cross Button            :rotation of the object
        Triangle Button         :rotation of the object
        Circle Button           :rotation of the object
        L1 Button               :transfer Z of the object
        R1 Button               :transfer Z of the object
        Select Button           :quit

<Remarks>

