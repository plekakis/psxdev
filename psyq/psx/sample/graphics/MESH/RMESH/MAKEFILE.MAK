DATDIR=..\..\..\..\data

all:
	ccpsx -O3 -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
        ccpsx -O3 -Xo$80080000 tuto0.c \psx\lib\none3.obj -ormesh.cpe
        cpe2x rmesh.cpe
        del rmesh.cpe

load:
	pqbload $(DATDIR)\tim\rcube.tim 80020000

