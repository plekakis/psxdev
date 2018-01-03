all:
	ccpsx -O -Xo$80100000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80100000 main.c \psx\lib\none2.obj -oxse.cpe
        cpe2x xse.cpe
        del xse.cpe

load:
	pqbload simple.vh 80010000
	pqbload simple.vb 80015000

clean:
	del *.?~
	del *.??~
	del *.cpe
	del *.sym
	del *.map
