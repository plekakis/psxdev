DATDIR=..\..\..\data

all:
	ccpsx -O3 -Xo$80080000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O3 -Xo$80080000 main.c \psx\lib\none3.obj -oppm.cpe
        cpe2x ppm.cpe
        del ppm.cpe

load:
	pqbload $(DATDIR)\tim\earth4.tim 80180000
