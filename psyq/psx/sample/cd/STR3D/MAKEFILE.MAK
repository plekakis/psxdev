DATDIR=..\..\..\data

all: tuto0
	
tuto0:
	ccpsx -O3 -Xo$80080000 tuto0.c cdadd.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -O3 -Xo$80080000 tuto0.c cdadd.c \psx\lib\none3.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe

load:
	pqbload $(DATDIR)\tim\pat16.tim 80010000
	pqbload $(DATDIR)\tim\pat8.tim  80020000
	pqbload $(DATDIR)\tim\pat4.tim  80030000
	pqbload $(DATDIR)\tmd\cube3.tmd 80040000
