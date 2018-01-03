DATDIR=..\..\..\..\data

all:
	ccpsx -g -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -g -Xo$80080000 tuto0.c \psx\lib\none3.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe

load:
	pqbload $(DATDIR)\tmd\shuttle1.tmd 80040000


