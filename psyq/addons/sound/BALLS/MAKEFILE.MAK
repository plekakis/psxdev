all:
        ccpsx -O -Xo$80100000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80100000 main.c \psx\lib\none2.obj -oballs.cpe
        cpe2x balls.cpe
        del balls.cpe


load:
	pqbload ..\simple\sample.vh 80020000
	pqbload ..\simple\sample.vb 80025000
	pqbload mozart.seq          80010000

clean:
	del *.?~
	del *.??~
	del *.cpe
	del *.sym
	del *.map
