$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

		FUNCTIONS FOR UNDISTORTED TEXTURE MAPPING 
		

draw_general_square	Performs texture mapping without square distortion.

    Syntax	draw_general_square(Vertex,ot,s,divn,txs0,txs1)
		SVECTOR Vertex[4];	/*3 dimensional coordinates for 4 vertices */
		u_long 	  *ot;		/* ＯＴHeader */
		POLY_FT4 *s;		/* Packet Area Header */
		int divn;		/* Number of lines for １division */
		int txs0;		/* Size of Texture（U direction）*/
		int txs1;		/* Size of Texture（V direction）*/

    Description  This function performs texture mapping of preset texture to the square 
		specified by Vertex[4].  The texture will not be distorted no matter
		where the four vertices locate.
		Within single execution, multiple square packets are generated and all 
		of them are linked to the same location in the OT.
		
		For the packet area, 2 times of the screen size in V direction divided 
 		by "divn"would be enough. Larger the "divn" is, more distortion you will 
		get. Texture size can be changed by "txs0" or "txs1".

		<Arguments Format>
		Vertex[i] : (1,15,0)
		
		<Note>
		Need to adjust the constant values depending on the Vertex size
						-> refer to the source program

    Return value 	None




draw_ground_square	Performs texture mapping without square distortion

    Syntax	draw_ground_square(Vertex,ot,s,txs0,txs1)
		SVECTOR Vertex[4];	/*3 dimensional coordinates for 4 vertices */
		u_long *ot;		/* OT Header */
		POLY_FT3 *s;		/* Packet Area Header*/
		int txs0; 		/* Size of Texture（U direction）*/
		int txs1;		/* Size of Texture（V direction）*/

Description	This function performs texture mapping of preset texture to the square 
		specified by Vertex[4].  The texture will not be distorted ( ex. bank-
		less road) when four vertices are on the locations where XZ plane (plane  
                 vertical to Y-axis) rotating in the order of Y-axis then X-axis. 

 		Within single execution, multiple triangle packets are generated and 
                 all of them are linked to the same location in the OT.

		One packet is generated for every scan line.
		For the packet area, the screen size in the V direction  would  be 
		enough.  Texture size can be changed by "txs0" or "txs1".

<Arguments Format>
		Vertex[i] : (1,15,0)
		
		<Note>
		Need to adjust the constant values depending on the Vertex size
						-> refer to the source program

Return value 	None







draw_ground_square2	Performs texture mapping without square distortion

    Syntax	draw_ground_square2(Vertex,ot,s,divn,txs0,txs1)
		SVECTOR Vertex[4];	/*3 dimensional coordinates for 4 vertices */
		u_long *ot;		/* OT Header */
		POLY_FT4 *s;		/* Packet Area Header */
		int divn;		/* Number of lines for １division */
		int txs0;		/* Size of Texture（U direction）*/
		int txs1;		/* Size of Texture（V direction）*/

Description	This function performs texture mapping of preset texture to the square 
		specified by Vertex[4].  The texture would not be distorted ( ex. bank-
                 less road) when four vertices are on the locations where XZ plane (plane  
                 vertical to Y-axis) rotating in the order of Y-axis then X-axis. 

                 Within single execution, multiple square packets are generated and
                 all of them are linked to the same location in the OT.

		One packet is generated for every *divn scan line.
		For the packet area, of the screen size in V direction divided by "divn" 
		would be enough. Texture size can be changed by "txs0" or "txs1".

<Arguments Format>
		Vertex[i] : (1,15,0)
		
		<Note>
		Need to adjust the constant values depending on the Vertex size
						-> refer to the source program

Return value 	None
		





How to Use the Sample Program

	PADRup/down: 	Rotation about X-axis
	PADRleft/right: 	Rotation about Y-axis
	PADm/o: 		Rotation about Z-axis
        PADLup/down: 	Translating in Y-axis direction
        PADLleft/right:	Translating in X-axis direction
        PADl/n:		Translating in Z-axis direction
        PADh&PADm:	Increment of line division number（For new algorithm）
        PADh&PADo:	Decrement of line division number (For new algorithm）
        PADh&PADm:	Increment of polygon division number（For old algorithm） 
        PADh&PADo:	Decrement of polygon division number（For old algorithm）
        PADh&PADl:	Mode selection 
        PADh&PADn:	Mode selection 
        PADm&PADl:	Texture address modification
        PADm&PADn:	Texture address modification
        PADo&PADn:	Texture address modification
        PADo&PADl:	Texture address modification
        PADk:		Terminate

Commenting out "#define INTERLACE" of text.c main.c  makes it non-interlace mode.

When running as the interlace mode, 60 frames/second cannot be accomplished depending 
on the state of the square and division number.   	

Other Considerations
	
Only draw_genral_square does the normal clipping.  This may sometime produce 
an incorrect texture mapping from close to the horizontal view point.
The shape of the squares also may not be correct from close to the horizontal view point.  
This is caused by calculation error.
First three of the four vertices are used for the texture address calculation.
Thus the square specified by Vertex[4] will be a part of parallelogram.
Packet area size to be prepared is determined by the division number and the   
screen resolution.
Please preset the texture address, CLUT, and the texture page.

For most cases, draw_ground_square can draw faster than draw_ground_square2.

