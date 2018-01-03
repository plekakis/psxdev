DATDIR=..\..\..\..\data

all: tuto0 tuto1 tuto2 tuto3 tuto4 tuto5 tuto6
	
tuto0:
	ccpsx -G 0 -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym
tuto1:
	ccpsx -G 0 -Xo$80080000 tuto1.c -otuto1.cpe,tuto1.sym
tuto2:
	ccpsx -G 0 -Xo$80080000 tuto2.c -otuto2.cpe,tuto2.sym
tuto3:
	ccpsx -G 0 -Xo$80080000 tuto3.c -otuto3.cpe,tuto3.sym
tuto4:
	ccpsx -G 0 -Xo$80080000 tuto4.c -otuto4.cpe,tuto4.sym
tuto5:
	ccpsx -G 0 -Xo$80080000 tuto5.c -otuto5.cpe,tuto5.sym
tuto6:
	ccpsx -G 0 -Xo$80080000 tuto6.c -otuto6.cpe,tuto6.sym

for_menu:
	ccpsx -G 0 -Xo$80080000 tuto0.c \psx\lib\none3.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del   tuto0.cpe
	
	ccpsx -G 0 -Xo$80080000 tuto1.c \psx\lib\none3.obj -otuto1.cpe,tuto1.sym
	cpe2x tuto1.cpe
	del   tuto1.cpe

	ccpsx -G 0 -Xo$80080000 tuto2.c \psx\lib\none3.obj -otuto2.cpe,tuto2.sym
	cpe2x tuto2.cpe
	del   tuto2.cpe

	ccpsx -G 0 -Xo$80080000 tuto3.c \psx\lib\none3.obj -otuto3.cpe,tuto3.sym
	cpe2x tuto3.cpe
	del   tuto3.cpe

	ccpsx -G 0 -Xo$80080000 tuto4.c \psx\lib\none3.obj -otuto4.cpe,tuto4.sym
	cpe2x tuto4.cpe
	del   tuto4.cpe

	ccpsx -G 0 -Xo$80080000 tuto5.c \psx\lib\none3.obj -otuto5.cpe,tuto5.sym
	cpe2x tuto5.cpe
	del   tuto5.cpe

	ccpsx -G 0 -Xo$80080000 tuto6.c \psx\lib\none3.obj -otuto6.cpe,tuto6.sym
	cpe2x tuto6.cpe
	del   tuto6.cpe


load:
        pqbload $(DATDIR)\tim\pat16t.tim 80010000
	pqbload $(DATDIR)\tim\pat8t.tim  80020000
	pqbload $(DATDIR)\tim\pat4t.tim  80030000
	pqbload $(DATDIR)\tmd\cube4t.tmd 80040000

load5:
        pqbload $(DATDIR)\tim\pat16t.tim 80010000
	pqbload $(DATDIR)\tim\pat8t.tim  80020000
	pqbload $(DATDIR)\tim\pat4t.tim  80030000
	pqbload $(DATDIR)\tmd\cube3.tmd  80040000

