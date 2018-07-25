$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

							
Sample Program for the Access to KANJI Font Data     version 2. 0
                                                            July 7, 1995

< Overview >
 This sample program uses each size KANJI data file. Since each KANJI 
 data file has its label, it is called in an application, and each KANJI data
 is accessed with the label.  

	11-dot non-KANJI        	unsigned long got11j0b[]
	11-dot First level KANJI 	        unsigned long got11j1b[]
	11dot Second level KANJI	unsigned long got11j2b[]
	11-dot Vertical writing	        unsigned long got11tat[]
	11-dot Half-size           	unsigned long got11hlf[]

	13-dot non-KANJI        	unsigned long got13j0b[]
	13-dot First level KANJI  	unsigned long got13j1b[]
	13-dot Second level KANJI	unsigned long got13j2b[]
	13-dot Vertical writing    	unsigned long got13tat[]
	13-dot Half-size           	unsigned long got13hlf[]

	15-dot non-KANJI        	unsigned long got15j0b[]
	15-dot First level KANJI  	unsigned long got15j1b[]
	15-dot Second level KANJI	unsigned long got15j2b[]
	15-dot Vertical writing    	unsigned long got15tat[]
	15-dot Half-size           	unsigned long got15hlf[]

	15-dot User-defined characters	unsigned long got15gai[]

  * Note: As KANJI font data, "..\fontdata\" file are referred to.

< How to use >
  First compile with psymake, and then start up with run main.

