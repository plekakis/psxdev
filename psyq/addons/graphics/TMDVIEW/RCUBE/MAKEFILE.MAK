all:
	ccpsx -g -Xo$80080000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -g -Xo$80080000 main.c \psx\lib\none2.obj -omain.cpe,main.sym
	cpe2x main.cpe
	del main.cpe
load:
	pqbload rcube.tmd 80010000
	pqbload rcube.tim 80020000
