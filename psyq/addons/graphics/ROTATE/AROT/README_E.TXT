$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

			ROTATION MATRIX INTERPOLATION PROGRAM
			
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

<How to operate>
	PADRleft/right:		Rotate about Y axis
	PADRup/down:		Rotate about X axis
	PADLleft/right:		Translation in X axis direction
	PADLup/down:		Translation in Y axis direction
	PADl:			Start interpolation
	PADk:			Terminate

