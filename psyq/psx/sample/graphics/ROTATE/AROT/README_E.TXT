$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
ROTATION MATRIX INTERPOLATION PROGRAM

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

			
 <Description>
	This Program interpolates with the rotation about an axis between unit matrix
	and an arbitrarily given rotation matrix.
	The status at the start of the program corresponds to the unit matrix.
	From this state, it will create any desirable state by rotating a cube. 
	This state can be described by a rotating matrix.
	Interpolation will be done from the initial state to final state by pressing 
	"PADl" button.	This sample program can change two states from initial  
	to final by finding the rotation axis, not by interpolating rotation angle or
	matrix elements.
	Therefore, it will not be a distorted interpolation resulting from rotation
	direction changes or from size/shape changes.

<How to Execute>
        psymake   	compile
        run main       	run

<Controller Button Assignment>
        Up Directional Button   :Translation in Y axis direction
        Down Directional Button :Translation in Y axis direction
        Right Directional Button:Translation in X axis direction
        Left Directional Button :Translation in X axis direction
        Square Button           :Rotate about Y axis
        Cross Button            :Rotate about X axis
        Triangle Button         :Rotate about X axis
        Circle Button           :Rotate about Y axis
        R1 Button               :Start interpolation
        Select Button           :Quit


