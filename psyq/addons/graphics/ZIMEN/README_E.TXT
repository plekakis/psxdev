$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1995.06.22
							1996.04.27
	
	  	    The ground (called zimen in Japanese)
	
	    Copyright (C) 1994-1996, Sony Computer Entertainment
			    All right reserved
	
How to startup
----------------------------------------	
	psymake 
	run tuto0
	run tuto1
	run tuto2
	run tuto3
	run tuto4
	run tuto5

Operation guide
----------------------------------------	
tuto0-tuto4:
    Left cross key
       Up       Move forward
       Down     Move backword
       Right    Move right 
       Left     Move left 

    Right cross key
       Up       Turn to  upword
       Down     Turn to downword
       Right    Turn to right
       Left     Turn to left

    Left side key
       2        Move downword
       1        Move upward

    Right side key
       2        rotate right
       1        rotate left
	
tuto5:
    Left cross key
       Up       Translates the view point upward
       Down     Translates the view point downward
       Right    Translates the view point toward the right
       Left     Translates the view point toward the left

    Right cross key
       Up       Not used
       Down    Not used
       Right    Rotates the direction of the view point clockwise
       Left     Rotates the direction of the view point counterclockwise 

    Left side key
       2        Increases the range of vision against the ground 
       1        Decreases the range of vision against the ground

    Right side key
       2        Backs the view point
       1        Advances the view point
	
Description
----------------------------------------	
tuto0	Primitive automatic division
	
	POLY_FT4 primitive is divided automatically in mesh form.
        The primitive is divided at the center point of each side into
        four quadrangles. 

        When the more division is designated, this processing will be 
        repeated for the divided quadrangles. 
        If every vertex of the divided quadrangle is outside the drawing 
        area at the time of recursive division, the division will not be 
        performed any more.  
	
        The division is expected to produce the following effects on the 
        primitive close to the screen. 
	
        * The primitives overflowing the drawing area are clipped 
          in advance, and this will reduce the overhead.

        * When the primitives get quite big by closing to the view 
          point, they will not be clipped in drawing.

        * A diagonal line distortion caused by the linear texture mapping
          will be reduced. 
	
	The division sample program is in div.c.
        This automatic division sample is provided for explaining the 
        operation. Use the DivPolyFT4() function for speeding-up the 
        processing.
	
		
tuto1	Area clipping

       The primitives placed on the z = 0 plane in the mesh form are clipped.
       The clipped area is obtained by converting the lines passing 
       through each vertex of the screen and the view point into the world
       coordinate system, and by finding the intersections of them and    
       the z = 0 plane.
       For example, if 640x480 mode is used, the screen location will be
       as follows in the screen coordinate system:
	
		(-320,-240,SCR_Z)-(320,240,SCR_Z)

       After this is converted into the world coordinate system, the view 
       point location in the world coordinate system and the intersection 
       of the line connecting each vertex of the screen and the z = 0 plane 
       will be obtained. The calculated area, enclosed by the four points, 
       will be displayed after coordinate and transparent perspective 
       conversion  (RotTransPers()).
       Refer to clip.c for more details.
	
      The screen_to_matrix() function performs conversion from the
      screen coordinate system into the world coordinate system .

      The get_z_cross_point() function finds the intersection of the 
      lines passing through the specified two points and the z = 0 plane.

       The rot_trans_pers() function performs transparent perspective
       conversion after converting the coordinate values from the world
       coordinate system to the screen coordinate system. Unlike
       RotTransPers(), GTE offsetting has no effect on this function.
	
       The y = 0 or x = 0 plane can be also used with a little change making
       use of the symmetry of the coordinate systems.

       The rectangle located in the center of the display is the screen.
       The rectangle outside the screen rectangle encloses the image 
       projected onto the z = 0 plane.

	
tuto2	Ground with a limited map

       The ground is expressed with the limited size in the mesh form, 
       and this means it has the ends.
       The texture pattern corresponding to each mesh cell is defined 
       by the map data. This is similar to the data structure of the 
       usual 2-dimensional scroll BG (background) basically.

       The ground mesh is a 2-dimensional array of the SVECTOR 
       structure, and the ground with the height can be expressed
       by operating the vz member (default is 0). Refer to mesh.c for
       more details. 

       The mesh cells are clipped in two steps, in the world coordinate 
       system and in the screen coordinate system. The clipped area in  
       the world coordinate system is designated on RECT32 *clipw. The 
       rotation and transparent perspective conversion of the mesh cells 
       in clipw are performed, and the second step will be performed if 
       the result is in RECT *clips in the screen coordinate system.

       First the mesh cell is clipped if its all vertices are outside the 
       clipw, and the coordinate conversion is performed if it is inside 
       the clipw. Also, the converted 2-dimensional cells are 
       clipped if their all vertices are outside the clips. When any one 
       vertex of the mesh cell is inside the clipping area, the cell will 
       be registered to OT.

       This mesh sample is provided for explaining the operation.
       Use the RotMesh function, at least the RotTransPersN() function, 
       for speeding-up the processing. 

	
tuto3	Ground with endless map

       By lapping round the translation vectors by the same size as the 
       1 map character, and Shifting the location of the map at the timing
       of lap-round, the endless repeated pattern will be created.

       This model has no end. However, the logical coordinate values
       are different from the physical coordinate values of the view point
       (camera) in the world coordinate system.

	
tuto4	Ground with the automatic division

       Division ON/OFF and the number after division corresponding to the 
       OT entry value sz are switched appropriately.
       The div_addprim() function performs it.

	
tuto5	CLUT FOG

        This is a coding example putting together the above processing.
        The functionality is added that the CLUT entry is switched 
        according to the value of each primitive depth queue (p), and
        CULT FOG is performed.
        Refer to the following explanation for details.

	
Coordinate Conversion

       In every sample the ground is assumed to be the z = 0 plane in the
       world coordinate system because of the ease of the operation of 
       the map data, and the view point location is assumed to be any 
       z > 0 point in the world coordinate system. For these reasons, it 
       is necessary to note when converting into the screen coordinate 
       system.
	
       The ground is fixed in the world coordinate system, and the view
       point translates in it. To achieve this the translation vectors 
       must be set as the world coordinates of the view point are always 
       converted to the origin (0, 0, 0) by the RotTrans() function. The 
       function performs the translation after the rotation.  
	
	x: world coordinate, R: rotation matrix , t: translation vector
	
       If the settings are as described above, the function  will 
       calculate as follows:
	
		 y = Rx + t
	
       If y is set at 0, it will calculate t = -Rx. x is the world 
       coordinate of the view point. t is the translation vector value.
	
       Moreover, the shift direction of the view point ( the time lag of 
       the view point) is described in the screen coordinate system because 
       it is more natural for the shift direction designated by the 
       controller to be set in the screen coordinate system. However, the 
       view point location itself is processed more easily in the world 
       coordinate system. 
       Thus, once the shift direction set in the screen coordinate system 
       is converted into the world coordinate, it must be added to the 
       view point coordinates in the world coordinate.
	
       The world coordinate can be calculated from the screen coordinate
       (inverse conversion of the RotTrans() function) as follows:	
	
	x = RR(y - t) = RRy - RRt	    RR is a inverse matrix of R.

       The rotation matrix can be calculated easily because the inverse 
       matrix of R is equal to the transposed matrix of R.      

tuto5.c
-------------------------------------------------------	
<< Overview >>

In this sample the 2-dimensional BG is rotated about the x axis until
it looks like the real.

The height has not been expressed, but it is useful enough to express 
the ground image on the flat plane. Also, since the source is provided 
as a sample, it can be modified as you like and installed.

As for the load, the routine calculating the ground with the default 
parameter set in the sample will consume 30 to 70 scanning lines. The 
load greatly depends on the parameter settings, with which the range of vision or the rate of the automatic division can be changed.
Therefore, the parameters, including the load, must be set for each 
application in detail.   

The following functionality is available.

* Automatic division by the polygon size and by the z value of the view 
  point coordinate, and the designation of the number after division
  by the z value
* Lap-round of the map data
* Depth queuing per polygon
* Adoption of the map data common with sample/graphics/tuto/tuto8.c
* Area clipping for speeding-up
* Subjective view point shift
* Endless plane in 32-bit space
 
The following functionality will be implemented, but not yet.

* Switching the texture source according to the depth
* Much more speeding-up
* Expression of the height
* Introduction of the mesh data structure
* Depth queuing per vertex



<< Parameter Explanation >>

*Parameter to divide     Definition of requirements for automatic 
                             division

typedef struct
{
  u_long nearz;
  u_long shift;
  u_long nz;
  u_long *cond;
} GsDIVCOND0;

	nearz:  Divides the cell automatically which has the z value
                 smaller than this value in the perspective coordinate 
                 system.
	shift:  Shifts the z value by this value and obtains the index 
                 of the division number table.
	nz   :  Number of the elements of the division number table
	cond :  Pointer to the division number table

The division number table is an array of u_long, and the number is designated with the second power. For example, 2x2 division is 1, 4x4 division is 2. 

* Ground Object Handler        Definition of the ground

typedef struct
{
  GsCOORDINATE2 *coord2;
  u_long cw,ch;			/* Width of cell */
  u_long nw,nh;			/* Number of displayed cells */
  u_long iw,ih;			/* Number of cells in 1 patternÅi2^nÅj*/
  GsDIVCOND0 *cond;
}  Gs3DBG0;

	coord2:  Pointer to the coordinate system including the ground
	cw,ch :  Length and width of a cell composing the ground
	nw,nh :  Size of the displayed ground (per cell) 
	iw,ih :  Unit of repeat (size of the map data)
                Designated with the valid bit number. For example, when 
                16x16 cells are repeated, iw = 0x0f and ih = 0x0f. When 
                32x32, iw = 0x1f, ih = 0x1f.
	        Consequently only the second power can be designated as
                the unit of repeat.
	cond  :  Pointer to the automatic division parameter GsDIVCONDO

* Map Data              Definition of the order of the cells of the ground

Refer to sample/graphics/tuto/tuto8.c because it has the same structure.
The map data depends on the applications. Thus, when the map data 
containing the different structure is used, the source can be changed for
better use. 


<< Tips for using >>

Division is performed for the following purposes.
 1 Preventing the lack of the polygon moving closer.
 2 Preventing the texture distortion of the polygon getting too big
   by moving closer.  
The number after division must be increased as the view point is closer to the ground. Division is performed by the z value, and the texture will move if the division/non-division are switched close by the viewpoint. If this movements are concerned, they will be less by increasing the parameter nearz and by dividing it in the distance.

Note: The division parameters settings to improve this quality have much
      effect on the processing speed.

In order to improve the quality the texture source per cell can
be switched according to the enlargement rate. In this sample, the texture
size of the cell is 32x32, and this is enlarged too much. Around 64x64 will be better.  

<< Bug >>
Since the area clipping functionality has not work perfectly, some
margin is set. If the end is clipped, change the AM value (1 at present)
to 2. 
This functionality will work worse as the distance between the viewpoint 
and the origin increases.  


< CLUT FOG >

CLUT FOG is the method to achieve the Fog functionality by changing the clut according to the depth. Normal fog functionality renders the textured polygon darker. However, it can blend with any color by the use of CLUT FOG. 

<< Parameter Explanation >>

* CLUT FOG parameter

typedef struct
{
  u_short *clut;
  u_short cbit;
  RECT    rectc;
  CVECTOR bgc;
} GsDPCLUT0;

	clut  :  Pointer of the original CLUT
	cbit  :  Number of CLUTs changed according to the depth
                ( per second power)
	rectc :  Address of the loaded CLUT on the VRAM
	bgc   :  Color to blend

The clut used in CULT FOG will be provided on the VRAM by calling the 
GsMakeDPClut0() function with CLUT FOG parameter as the argument.
The clut is created below the original clut directing toward y. 
(See the following figure.)

	                 VRAM		CBIT = 2
     Ñ°ÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑ¢
     Ñ† 			               	 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ•ÑüÑüÑüÑüÑüÑüÑüÑ¢original clut		 Ñ†
     Ñ•ÑüÑüÑüÑüÑüÑüÑüÑßclut 1	    		 Ñ†
     Ñ•ÑüÑüÑüÑüÑüÑüÑüÑßclut 2	    		 Ñ†
     Ñ•ÑüÑüÑüÑüÑüÑüÑüÑßclut 3	    		 Ñ†
     Ñ•ÑüÑüÑüÑüÑüÑüÑüÑ£		    		 Ñ†
     Ñ†				    		 Ñ†
     Ñ§ÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑüÑ£

<< Function Explanation >>

* GsMakeDPClut0(GsDPCLUT0 *ClutHandle)	
           Creating CLUT FOG and loading it to VRAM

This function creates CLUT FOG referring to the parameter set in the 
GsDPCLUT0() function, and transfers it to the rectc address on the VRAM.
Note that the rectc area is rewritten. 

In order to draw the texture polygon with this CLUT FOG the clut address 
must be switched by the p value when creating a packet. Offsetting only 
the y address from the original CLUT will achieve the better-blended CLUT.
Refer to the sample for more details. 


