DATDIR=..\..\..\data

all:
	ccpsx -O3 -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -O3 -Xo$80080000 tuto0.c \psx\lib\none3.obj -omipmap.cpe
	cpe2x mipmap.cpe
	del mipmap.cpe

load:
	pqbload $(DATDIR)\tmd\block.tmd 80100000
	pqbload $(DATDIR)\tim\block.tim 80120000
