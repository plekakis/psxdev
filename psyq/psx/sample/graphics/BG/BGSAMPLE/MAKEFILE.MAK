DATDIR=..\..\..\..\data

all:
	ccpsx -g -Xo$80080000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -g -Xo$80080000 \psx\lib\none3.obj main.c -obgsample.cpe
	cpe2x bgsample.cpe
	del   bgsample.cpe

load:
	pqbload $(DATDIR)\tim\bgsample.tim 80180000
	pqbload $(DATDIR)\bgd\bgsample.bgd 80100000
	pqbload $(DATDIR)\cel\bgsample.cel 80140000
