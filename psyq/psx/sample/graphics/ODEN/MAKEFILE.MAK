DATDIR=..\..\..\data

all:
	ccpsx -O -Xo$80080000 oden.c -ooden.cpe,oden.sym

for_menu:
	ccpsx -O -Xo$80080000 \psx\lib\none3.obj oden.c -ooden.cpe
	cpe2x oden.cpe
	del   oden.cpe

load:
        pqbload $(DATDIR)\tim\oden.tim 801c0000
	pqbload $(DATDIR)\tmd\oden.tmd 80040000

