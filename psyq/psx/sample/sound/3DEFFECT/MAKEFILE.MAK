DATDIR=..\..\..\data

all:
        ccpsx -O -Xo$80100000 main.c menu.c -omain.cpe,main.sym

for_menu:
	ccpsx -O -Xo$80100000 main.c menu.c \psx\lib\none3.obj -o3d.cpe
	cpe2x 3d.cpe
	del 3d.cpe

load:
        pqbload $(DATDIR)\vab\demo.vh  80020000
        pqbload $(DATDIR)\vab\demo.vb  80025000

clean:
	del *.cpe
	del *.sym
