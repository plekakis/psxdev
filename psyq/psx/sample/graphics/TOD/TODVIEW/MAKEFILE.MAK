DATDIR=..\..\..\..\data

all:
	ccpsx -O -Xo$80010000 main.c todanim.c objctl.c -omain.cpe,main.sym,main.map

for_menu:
	ccpsx -O -Xo$80010000 \psx\lib\none3.obj main.c todanim.c objctl.c -otodview.cpe
	cpe2x todview.cpe
	del   todview.cpe

load:
	pqbload $(DATDIR)\tmd\te.tmd      800c0000
	pqbload $(DATDIR)\tod\jaanken.tod 800e0000
