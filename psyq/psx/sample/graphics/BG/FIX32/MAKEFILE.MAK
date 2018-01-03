DATDIR=..\..\..\..\data

all:
	ccpsx -g -Xo$80080000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -g -Xo$80080000 \psx\lib\none3.obj main.c -ofix32.cpe
	cpe2x fix32.cpe
	del   fix32.cpe

load:
	pqbload $(DATDIR)\tim\fix32.tim 80180000
	pqbload $(DATDIR)\cel\fix32.cel 80140000
	pqbload $(DATDIR)\bgd\fix32_0.bgd 80100000
	pqbload $(DATDIR)\bgd\fix32_1.bgd 80101000
	pqbload $(DATDIR)\bgd\fix32_2.bgd 80102000
	pqbload $(DATDIR)\bgd\fix32_3.bgd 80103000
