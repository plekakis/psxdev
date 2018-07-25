DATDIR=..\..\..\..\data

all: tuto0 tuto1 tuto2 tuto3
	

OBJ1=g4l.obj tg4l.obj g4lfg.obj tg4lfg.obj g4nl.obj tg4nl.obj
OBJ2=g4trr.obj tg4trr.obj
CC = ccpsx -G 0 -Xo$80080000 -O3
CC2 = ccpsx -Xo$80080000 -g

tuto0:
	$(CC) tuto0.c -otuto0.cpe,tuto0.sym

tuto1:	$(OBJ1)
	$(CC) tuto1.c $(OBJ1) -otuto1.cpe,tuto1.sym

tuto2:	$(OBJ2)
	$(CC) tuto2.c $(OBJ2) -otuto2.cpe,tuto2.sym

tuto3:	tg4lmip.obj
	$(CC) tuto3.c tg4lmip.obj -otuto3.cpe,tuto3.sym

g4l.obj: 
	$(CC) -c g4l.c
	dmpsx g4l.obj

tg4l.obj: 
	$(CC) -c tg4l.c
	dmpsx tg4l.obj

g4lfg.obj: 
	$(CC) -c g4lfg.c
	dmpsx g4lfg.obj

tg4lfg.obj: 
	$(CC) -c tg4lfg.c
	dmpsx tg4lfg.obj

g4nl.obj: 
	$(CC) -c g4nl.c
	dmpsx g4nl.obj

tg4nl.obj: 
	$(CC) -c tg4nl.c
	dmpsx tg4nl.obj

g4trr.obj: 
	$(CC) -c g4trr.c
	dmpsx g4trr.obj

tg4trr.obj: 
	$(CC) -c tg4trr.c
	dmpsx tg4trr.obj

tg4lmip.obj: 
	$(CC) -c tg4lmip.c
	dmpsx tg4lmip.obj

for_menu: g4l.obj tg4l.obj g4lfg.obj tg4lfg.obj g4nl.obj tg4nl.obj g4trr.obj tg4trr.obj tg4lmip.obj
	ccpsx -G 0 -Xo$80080000 tuto0.c \psx\lib\none3.obj -otuto0.cpe,tuto0.sym
	ccpsx -G 0 -Xo$80080000 tuto1.c \psx\lib\none3.obj g4l.obj tg4l.obj g4lfg.obj tg4lfg.obj g4nl.obj tg4nl.obj -otuto1.cpe,tuto1.sym
	ccpsx -G 0 -Xo$80080000 tuto2.c \psx\lib\none3.obj g4trr.obj tg4trr.obj -otuto2.cpe,tuto2.sym
	ccpsx -G 0 -Xo$80080000 tuto3.c \psx\lib\none3.obj tg4lmip.obj -otuto3.cpe,tuto3.sym
	cpe2x tuto0.cpe
	cpe2x tuto1.cpe
	cpe2x tuto2.cpe
	cpe2x tuto3.cpe
	del tuto0.cpe
	del tuto1.cpe
	del tuto2.cpe
	del tuto3.cpe

load:
	pqbload $(DATDIR)\tim\pat16.tim 80120000
	pqbload $(DATDIR)\tim\pat8.tim  80140000
	pqbload $(DATDIR)\tim\pat4.tim  80160000
	pqbload $(DATDIR)\tmd\cube3.tmd  80100000

load2:
	pqbload $(DATDIR)\tim\pat16.tim 80120000
	pqbload $(DATDIR)\tim\pat8.tim  80140000
	pqbload $(DATDIR)\tim\pat4.tim  80160000
	pqbload $(DATDIR)\tmd\cube4g.tmd  80100000

load3:
	pqbload $(DATDIR)\tmd\cube4gm.tmd  80100000
	pqbload $(DATDIR)\tim\pat16m0.tim  80120000
	pqbload $(DATDIR)\tim\pat16m1.tim  80140000
	pqbload $(DATDIR)\tim\pat16m2.tim  80160000

