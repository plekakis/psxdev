DATDIR=..\..\..\..\data

tuto0:
	ccpsx -c -O4 tuto0.c -otuto0.o
	ccpsx -c -O4 datg4_cm.c -odatg4_cm.o
	dmpsx datg4_cm.o
	ccpsx -Xo$80010000 -O4 tuto0.o datg4_cm.o -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -c -O4 tuto0.c -otuto0.o
	ccpsx -c -O4 datg4_cm.c -odatg4_cm.o
	dmpsx datg4_cm.o
	ccpsx -Xo$80010000 -O4 \psx\lib\none3.obj tuto0.o datg4_cm.o \
						-otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe
	
load:
	pqbload $(DATDIR)\tim\earth8.tim 80100000
