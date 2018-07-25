$PSDocId: Document Release 1.0 for Runtime Library Release 3.6$

	conbination 3D graphics & movie


[functions]
	This is the program for mapping streamed texture or streamed BG.

	This program is conbination TMDVIEW & ANIM, TMDVIEW program is
	main code and ANIM program is polled for sub code.

[files]

	cdadd.c	tuto0.c
	cube3.tmd pat4.tim pat8.tim pat16.tim
	makefile.mak

[how to make]
	psymake
	psymake load

[key assign]
	left arrow key		transfer of the object
	right arrow key		rotation of the object

[comments]
	If you define BG in tuto0.c , streamed data is mapped for BG.
	If you define TEXTURE in tuto0.c , streamed data is mapped for
	16 bit texture of the object.

	In library version 3.2 later , you can call DecDCTvlcSize().
	This function limits the size of VLC decode at one time , so
	you can control the load for decodeing.

	If you set the size of limitation of VLC decoding by calling
	DecDCTvlcSize larger , animation will be smooth but 3D graphics
	frame rate will be reduced.
	On the other hand , if you set the size smaller , animation will
	be not smooth but 3D part will be smooth.
	Please decide the size of VLC decoding case by case.


