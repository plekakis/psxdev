DATDIR=..\..\..\..\data

all:
	ccpsx -O -Xo$80010000 main.c nmime.c control.c -omain.cpe,main.sym
load:
	pqbload $(DATDIR)\tmd\md0.tmd 800d0000
	pqbload md0.vdf  80090000
	pqbload md0.ndf  800b0000

for_menu:
	ccpsx -O -Xo$80010000 main.c nmime.c control.c c:\psx\lib\none3.obj -omime_m.cpe,mime_m.sym
	cpe2x mime_m.cpe
	del mime_m.cpe
