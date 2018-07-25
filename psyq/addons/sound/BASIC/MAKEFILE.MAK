all:
	ccpsx -O -Xo$80100000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80100000 main.c \psx\lib\none2.obj -obasic.cpe
        cpe2x basic.cpe
        del basic.cpe

load:
	pqbload ..\simple\sample.vh 80030000
	pqbload ..\simple\sample.vb 80032a20
	pqbload sample.sep 80010000
	pqbload sinfonie.seq 80015000

clean:
	del *.cpe
	del *.sym
	del *.map
	del *.?~
	del *.??~
