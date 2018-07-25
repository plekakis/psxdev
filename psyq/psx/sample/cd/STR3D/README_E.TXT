$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.
	conbination 3D graphics & movie

<Description>
	This is the program for mapping streamed texture or streamed BG.

	This program is conbination TMDVIEW & ANIM, TMDVIEW program is
	main code and ANIM program is polled for sub code.

	If you define BG in tuto0.c , streamed data is mapped for BG.
	If you define TEXTURE in tuto0.c , streamed data is mapped for
	16 bit texture of the object.

	In library version 3.2 later , you can call DecDCTvlcSize().
	This function limits the size of VLC decode at one time , so
	you can control the load for decodeing.

	If you set the size of limitation of VLC decoding by calling
	DecDCTvlcSize larger , animation will be smooth but 3D graphics
	frame rate will be reduced.
	On the other hand , if you set the size smaller , animation will
	be not smooth but 3D part will be smooth.
	Please decide the size of VLC decoding case by case.

<How to Execute>
	> psymake
	> psymake load
	> run tuto0

<Controller Button Assignment>
        Up Directional Button   :	Translation The Object (UP)
        Down Directional Button :	Translation The Object (DOWN)
        Right Directional Button:	Translation The Object (RIGHT)
        Left Directional Button :	Translation The Object (LEFT)
        Square Button           :	Rotation The Object(LEFT)
        Cross Button            :	Rotation The Object(DOWN)
        Triangle Button         :	Rotation The Object(UP)
        Circle Button           :	Rotation The Object(RIGHT)
	R1,R2 Button            :       Moving Object in front 
                                        and behind 
        Select Button           :	Exit The Program

