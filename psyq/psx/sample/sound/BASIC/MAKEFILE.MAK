DATDIR=..\..\..\data

all:
	ccpsx -O -Xo$80100000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80100000 main.c \psx\lib\none3.obj -obasic.cpe
        cpe2x basic.cpe
        del basic.cpe

load:
	pqbload $(DATDIR)\vab\simple.vh 80030000
	pqbload $(DATDIR)\vab\simple.vb 80032a20
	pqbload $(DATDIR)\sep\sample.sep 80010000
	pqbload $(DATDIR)\seq\sinfonie.seq 80015000

clean:
	del *.cpe
	del *.sym
	del *.map
	del *.?~
	del *.??~
