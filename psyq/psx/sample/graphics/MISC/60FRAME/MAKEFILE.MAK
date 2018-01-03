DATDIR=..\..\..\..\data

all:
	ccpsx -O2 -Xo$800a0000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -O2 -Xo$800a0000 main.c \psx\lib\none3.obj -o60frame.cpe
	cpe2x 60frame.cpe
	del   60frame.cpe

load:
	pqbload $(DATDIR)\tim\giuli_bd.tim 80010000
	pqbload $(DATDIR)\tim\giuli_fr.tim 80020000
	pqbload $(DATDIR)\tim\giuli_pl.tim 80030000
	pqbload $(DATDIR)\tim\giuli_tl.tim 80040000
	pqbload $(DATDIR)\tim\giuli_tr.tim 80050000
	pqbload $(DATDIR)\tim\giuli_wn.tim 80060000
	pqbload $(DATDIR)\tim\wheel256.tim 80070000
	pqbload $(DATDIR)\tmd\giulieta.tmd 80080000

