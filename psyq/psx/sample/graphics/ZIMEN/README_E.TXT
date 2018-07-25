$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

				The Ground

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

<Description>

	- Here are simple tutorial samples to display infinite ground
	  plane .

	- Each demo is executed from MENU. see "menu.h" and "main.c"
	  for detail.

	- Each demonstration program can be extracted from menu. See
	  "main.c" for detail.

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
	
	In this sample the 2-dimensional BG is rotated about the x
	axis until it looks like the real.
	The height has not been expressed, but it is useful enough to
	express the ground image on the flat plane. Also, since the
	source is provided as a sample, it can be modified as you like
	and installed. 

	As for the load, the routine calculating the ground with the
	default parameter set in the sample will consume 30 to 70
	scanning lines. The load greatly depends on the parameter
	settings, with which the range of vision or the rate of the
	automatic division can be changed. 
	Therefore, the parameters, including the load, must be set for
	each application in detail.   
	
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


<How to Execute>
	
	>psymake main
	>run main

<Controller Button Assignment>
        Directional Button      :  move cursor
        Circle Button           :  execute
        Select Button           :  exit

<Remarks>
	- DEMO button indicates to start demonstration program.
	- HELP button indicates to display a help window.
	  Key assign of each demonstration is shown on this help window.
