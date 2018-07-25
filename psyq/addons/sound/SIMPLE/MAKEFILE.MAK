all:
	ccpsx -O -Xo$80100000 main.c -osimple.cpe,simple.sym
	ccpsx -O -Xo$80100000 jump.c -ojump.cpe,jump.sym

for_menu:
	ccpsx -O -Xo$80100000 main.c \psx\lib\none2.obj -osimple.cpe
	cpe2x simple.cpe
	del simple.cpe
	ccpsx -O -Xo$80100000 jump.c \psx\lib\none2.obj -ojump.cpe
	cpe2x jump.cpe
	del jump.cpe

load:
	pqbload sample.vh 80015000
	pqbload sample.vb 80020000
	pqbload ..\basic\fuga.seq 80010000

clean:
	del *.?~
	del *.??~
	del *.cpe
	del *.sym
	del *.map
