DATDIR=..\..\..\data

all:
        ccpsx -O -Xo$80100000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80100000 main.c \psx\lib\none3.obj -oballs.cpe
        cpe2x balls.cpe
        del balls.cpe


load:
	pqbload $(DATDIR)\vab\sample.vh 80020000
	pqbload $(DATDIR)\vab\sample.vb 80025000
	pqbload $(DATDIR)\seq\mozart.seq 80010000

clean:
	del *.?~
	del *.??~
	del *.cpe
	del *.sym
	del *.map
