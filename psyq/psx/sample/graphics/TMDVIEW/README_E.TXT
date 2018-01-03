$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

          TMDVIEWS: Sample programs for TMD-formatted models

               Copyright(C) 1998 Sony Computer Entertainment Inc.
                                             All Rights Reserved.

                       
TMDVIEW3	Viewer for GsDOBJ3 
                GsDOBJ3 handles PMD-format data.
		For the PMD format, the packet area is already 
                included within the model data, thus enabling 
                high-speed packet creation.
                

TMDVIEW4	Viewer for GsDOBJ2 
		If TMD data primitives are sorted in advance and 
                grouped together, high-speed packet creation will
                be possible. TMD primitive sorting is executed by 
                using the command tmdsort.

TMDVIEW5	Viewer for GsDOBJ5
		GsDOBJ5 quickly calculates packets by calculating
                the packets in advance and rewriting only the parts
                that have been changed.
                (this is called preset packet) 

		If primitives are sorted in advance, the calculations
                will be done much faster for this type of TMD also.

		tuto0 is a basic GsDOBJ5 viewer.
		tuto1 is a program which divides the screen into top 
                and bottom and displays the object seen from different
                view points.
                tuto2 is an example in which two ordering tables are 
                used to sort each object.
		tuto3 is an example of automatic division where 
