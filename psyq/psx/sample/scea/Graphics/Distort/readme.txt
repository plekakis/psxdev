Date:  Sept 25, 1998
-------------------------------------

File Name:
Distort.zip

-------------------------------------
Contents Overview:

This demo shows how to do motion blurring and a disortion overlay

-------------------------------------
Detailed Description:

History
=========================
None


Disortion and Mblur demo 
************************
This demo shows how to do motion blurring and a disortion overlay
Its not really finished but the code has gone cold and I decided to 
release as is, rather than go back and spend more time on it.
SELECT switches between mblur and mblur+distortion.

 ok this is broken in 2 places.... 
 if you leave it running for about 5 minutes one of the counters 
 flips negative and the distortion doodaahs go weird
 
 I did something stupid when I added the texture buffer ( its currently 
 under the yingyang symbol... see the comments in effects.c about how
 to change the size of the drawenvironment.  You can make it wider when 
 you need the offscreen buffer and then set it back afterwards....
 this is probably the best way to go....

Select switches between mblur and mblur+distortion overlay....

DCoombes98

SUPPORT :
        Please direct comments or questions to:

        David Coombes, Developer Support
        Sony Computer Entertainment America
        919 E. Hillsdale Ave.
        Foster City, CA  94404
        EMAIL: David_Coombes@playstation.sony.com

        Developer Hotline: (650) 655-8181

--------------------------------------------------------------
Copyright (C) 1998, 1999. Sony Computer Entertainment America Inc.

************************
