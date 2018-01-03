#
# file : makefile.mak
#
#	makefile for psymake
#
makecard.cpe: makecard.c
	ccpsx -Xo$80080000 -kanji makecard.c -o makecard.cpe,makecard.sym

for_menu: makecard.c
	ccpsx -Xo$80080000 -kanji \psx\lib\none3.obj makecard.c -o makecard.cpe,makecard.sym
	cpe2x makecard.cpe
	del   makecard.cpe

clean:
	del *.obj
	del *.cpe
	del *.sym

