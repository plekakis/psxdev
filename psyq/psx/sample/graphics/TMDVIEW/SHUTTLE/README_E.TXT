$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

	tuto0:  sample code for hiaracial coordinates using GsDOBJ5

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	This sample code is using the hiarcial coordinates.
	The parent coordinate is the frame of shuttle.
	The child coordinate are hatchs and satellite.

<How to Execute>
	
	psymake 	compile
	psymake load	data load
	run tuto0       run

<Controller Button Assignment>

        Up Directional Button   :	translation of the shuttle
        Down Directional Button :	translation of the shuttle
        Right Directional Button:	translation of the shuttle
        Left Directional Button :	translation of the shuttle
        Square Button           :	rotation of the shuttle
        Cross Button            :	rotation of the shuttle
        Triangle Button         :	rotation of the shuttle
        Circle Button           :	rotation of the shuttle
        L1 Button               :	translation of the satellite
        L2 Button               :	translation of the satellite
        R1 Button               :	open & close the hatch
        R2 Button               :	open & close the hatch
        Select Button           :	Exit Program

<Remarks>

[modeling data]

	shuttle1.tmd	shuttle data
			ID.0	frame of the shuttle
			ID.1	hatch
			ID.2	hatch
			ID.3	satellite
			ID.4	antena of the satellite (unused)

