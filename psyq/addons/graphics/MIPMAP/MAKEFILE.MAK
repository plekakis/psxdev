all:
	ccpsx -O3 -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -O3 -Xo$80080000 tuto0.c \psx\lib\none2.obj -otuto0.cpe
	cpe2x tuto0.cpe
	del tuto0.cpe

load:
	pqbload block.tmd 80100000
	pqbload block.tim 80120000
