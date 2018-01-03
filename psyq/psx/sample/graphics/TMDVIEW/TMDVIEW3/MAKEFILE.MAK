DATDIR=..\..\..\..\data

all:
	ccpsx -g -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -g -Xo$80080000 tuto0.c \psx\lib\none3.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe

load:
	pqbload $(DATDIR)\tim\pat.tim 80010000
	pqbload $(DATDIR)\pmd\share.pmd 80040000

