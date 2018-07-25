$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

		PS Tool Manual

File name:
	kanjidiv : Extracting data from KANJI font

Date:
	July 5, 1995 Version 1.2

Syntax:
	kanjidiv [options] input-file

Explanation:
        Extracts KANJI data specified in input-file from KANJI font data 
        (non-KANJI, the first level KANJI, the second level KANJI,
        user-defined characters) and outputs to a file.
	
        Note: Not corresponding to half-size characters and characters 
              for vertical writing.
              As for the user-defined characters, only 15x15 font is 
              available.   


	[options]

	-o output-file	Specifying the file name to which the 
                        extracted KANJI data is output.
			kanji.out is default.

	-font 11|13|15	Specifying the font of output KANJI data.
                        Slecting among 11x11, 13x13 and 15x15.
			15 is default.

	-path font-path	Specifying the path name of KANJI font data 
                         file.
                        "..\fontdata\\" is default.
			
	-v		Detail information

Remarks:
        KANJI font data is stored in "..\fontdata\got1[1,3,5]j[0,1,2]b.h"
        and in ".\fontdata\got15gai.h", and read when kanjidiv boots up .
        If the data is stored under other directories, specify the path 
        with -path option.

Example:
	kanjidiv -o kanji.out -font 15 test.knj
