$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

							1994.08.04 
							1995.03.17
							1996.01.25
				  PMD
	
	     Copyright (C) 1994-1996 Sony Computer Entertainment
			    All right reserved
	

	Here are sample programs for PMD packet viewer.
	PMD is 3D primitive data format for fast drawing. Since PMD
	data includes GPU primitive structure, fewer memory access is
	required for primitive making.
	
	rotPMD*() functions decodes PMD format. they only change x,y
	member of the primitve and do not touch r,g,b and u,v member.
	Therefore PMD does not have normal vector information for
	lighting calculation. Please use TMD foramt if real-time
	lighting is needed.

	See following files for detail information.

			doc/eng/format/pmd.txt

	
	        ================================================
		program		contents	
	        ------------------------------------------------
		tuto0		display PMD data
		tuto1		display TMD data as PMD
		tuto2		display TMD data as PMD
		tuto3		lighting with PMD
		tuto5		PMD display using inline RotPMD 
	        ------------------------------------------------

