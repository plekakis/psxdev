$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

	Active Subdive sample program

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

This sample is dividing actively 1 quad polygon, and additional topics are
cover polygon junctions and selecting sorting Zs.

You must pqbload earth8.tim 80100000
And run tuto0

In this program , the speed is the first priority, so the destortion of
texture is apparent. If you minimize the distortion , you must redifine
#define MAX_POLY_WIDTH (1023/4)	/* max polygon distance X */
#define MAX_POLY_HIGHT (511/4)	/* max polygon distance Y */
these value smaller , so that dividing number to increase.

<How to Execute>

	> psymake tuto0
	> run tuto0

<Controller Button Assignment>

        Up Directional Button   :	Transfer The Object(UP)
        Down Directional Button :	Transfer The Object(DOWN)
        Right Directional Button:	Transfer The Object(RIGHT)
        Left Directional Button :	Transfer The Object(LEFT)
        Square Button           :	Rotate The Object(LEFT)
        Cross Button            :	Rotate The Object(DOWN)
        Triangle Button         :	Rotate The Object(UP)
        Circle Button           :	Rotate The Object(RIGHT)
        R1 Button               :	Transfer The Object(NEARER)
        R2 Button               :	Transfer The Object(FARER)
        Select Button           :	Exit The Program

