SAMPLE	: FIRE
AUTHOR	: M. Koziniak
DATE	: 11-19-97
Copyright (C) 1997 by Sony Computer Entertainment. All rights Reserved.

LEVEL	: 
	This sample is suitable for beginners.

PURPOSE :
	This is an instructional program that illustrates how to make
	a procedural texture which can then be used multiple times.
	

DESCRIPTION :
	This program uses double buffering and the ordering table.

	The fire is built via two arrays whose size should not exceed the
	texture cache for 8bit. We also use a gradient pallette which is 
	used to determine the color of a fire pixel.  This gradient should
	roughly be the different colors of a fire.

	Lets call our two array's a start buffer and a screen buffer. We set
	the bottom line of the start buffer to contain random "hotspots."

	We then average that hotspot with all or some of its neighbors and
	we save the result in the pixel directly above it in the screen buffer.
	This allows the fire to spread and since the average wants to fall
	to zero the flame eventually dies out.

	This sample could be made faster by using the scratch pad. If you
	could spare the room you could copy three rows at a time and then
	process them.  Once you have built the texture it is of minimal cost
	to reuse it so one might want to stack fires on top of eachother and
	experiment with transparency.

SEE ALSO :

MAKING :
	>psymake

RUNNING :
	>psymake run

        PADRdown increment through availiable texture types
        PADRup decrement through availiable texture types

FILES :
	main.c - the only source code file for this sample
	main.h - contains the header information used by main
	clut.tim - The CLUT's which are used by the fire


SUPPORT :
	Please direct comments or questions to:

	Mike Koziniak, Technical Support
	Sony Computer Entertainment America
	919 E. Hillsdale Ave.
	Foster City, CA  94404
	EMAIL: michael_koziniak@playstation.sony.com

	Developer Hotline: (650) 655-8181
