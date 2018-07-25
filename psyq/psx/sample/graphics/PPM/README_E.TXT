$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
SAMPLE PROGRAM FOR FLAT TEXTURE SQUARE PERSPECTIVE MAPPING

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>
	This sample programs performs the texture mapping without distortion by
	perspective conversion of the square bit by bit.
	Currently, the function below in the jimen sample can perform faster texture
	mapping.
			draw_general_square


<How to Execute>
        psymake    	compile
        psymake load   	load data
        run main       	run

<Controller Button Assignment>
        Up Directional Button   :Translation in Y-axis direction
        Down Directional Button :Translation in Y-axis direction
        Right Directional Button:Translation in X-axis direction
        Left Directional Button :Translation in X-axis direction
        Square Button           :Rotation about Y-axis
        Cross Button            :Rotation about Z-axis
        Triangle Button         :Rotation about Z-axis
        Circle Button           :Rotation about Y-axis
        L1 Button               :Translation in Z-axis direction
        R1 Button               :Translation in Z-axis direction
        Select Button           :Quit

<Remarks>
