
Date: October 13, 1997.
Contents: This directory contains unsupported utilities for 
 	  doing movie converter routines and accessing the 
	  MDEC.

---------------------------------------------------------------

File:
	mvcnvTar.z 


Contents:
	mvcnvTar.z unsupported (for SGI IRIX 5.3) 
	Includes the following:
		menc - MDEC encoder for PlayStation
		str - creates streaming data for PlayStation
		unstr - extracts frame data from non-audio str file .
		catstr - concatenates non-audio str files to one non-audio str file.
		mixav - interleaves str data with xa audio data.
		readme.txt - English document

Installation:
1. uncompress mcnvTar.Z
2. tar xf mcnvTar
3. Copy contents of "bin" to some directory (/usr/local/bin for instance)
   Read the "readme.txt" for more details.
 
Note: Unsupported release for SGI

---------------------------------------------------------------


File:
	ver11.Z

Contents: 
	ver11.z (for SGI IRIX 5.3) - contains a library for accessing
	the MDEC encoding routines.  A sample file is
	included.  This will no longer be supported in the future. 

Installation:
	1. uncompress ver11.Z
	2. tar xf ver11
	3. 3 directories are created.  The contents of the include should
   		be placed in your "include" directory (such as /usr/local/include), and
   		the contents of the lib directory in your "lib" directory (such as
   		/usr/local/lib).  Compile and run the sample program in "sample".

================================================================

Copyright (C) 1998. Sony Computer Entertainment Inc.