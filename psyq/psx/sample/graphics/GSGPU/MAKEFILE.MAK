DATDIR=..\..\..\data
SRCS  = main.c tuto0.c tuto1.c
main:
	ccpsx -O3 -Xo$80020000 $(SRCS) -omain.cpe,main.sym
load:
	pqbload $(DATDIR)\tmd\mado.tmd 80100000
	pqbload $(DATDIR)\tim\mado.tim 80180000

for_menu:
	ccpsx -O3 -Xo$80020000 $(SRCS) \psx\lib\none3.obj -omain.cpe
	cpe2x main.cpe
	del main.cpe

