all:
	ccpsx -g -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -g -Xo$80080000 tuto0.c \psx\lib\none2.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe
load:
	pqbload pat.tim 80010000
	pqbload shared.pmd 80040000

