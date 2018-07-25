DATDIR=..\..\..\data

all:
	ccpsx -O2 -G0 -Xo$800c0000 main.c move.c maps.c maze.c sin.c -omain.cpe,main.sym

for_menu:
	ccpsx -O2 -G0 -Xo$800c0000 main.c move.c maps.c maze.c sin.c \psx\lib\none3.obj -omain.cpe,main.sym
	cpe2x main.cpe
	del main.cpe

load:
	pqbload $(DATDIR)\tmd\wall0.tmd 80090000
	pqbload $(DATDIR)\tmd\wall1.tmd 80090100
	pqbload $(DATDIR)\tmd\wall2.tmd 80090200
	pqbload $(DATDIR)\tmd\wall3.tmd 80090300
	pqbload $(DATDIR)\tmd\yuka.tmd  80090400
	pqbload $(DATDIR)\tim\moss.tim  80090800
	pqbload $(DATDIR)\tim\yuka.tim  80092000
