all:
	ccpsx -O -Xo$80100000 viewer.c -oviewer.cpe,viewer.sym

for_menu:
	ccpsx -O -Xo$80100000 viewer.c \psx\lib\none3.obj -oviewer.cpe
	cpe2x viewer.cpe
	del viewer.cpe

load:
	pqbload kanji.out 80010000

