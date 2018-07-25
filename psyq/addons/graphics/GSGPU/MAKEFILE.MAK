all: tuto0 tuto1
	
tuto0:
	ccpsx -O -Xo$80080000 tuto0.c -otuto0.cpe,tuto0.sym
tuto1:
	ccpsx -O -Xo$80080000 tuto1.c -otuto1.cpe,tuto1.sym

for_menu: tuto0.exe tuto1.exe
	
tuto0.exe:
	ccpsx -O -Xo$80080000 tuto0.c \psx\lib\none2.obj -otuto0.cpe
	cpe2x tuto0.cpe
tuto1.exe:
	ccpsx -O -Xo$80080000 tuto1.c \psx\lib\none2.obj -otuto1.cpe
	cpe2x tuto1.cpe
load:
	pqbload mado.tim 80180000
	pqbload mado.tmd 80040000
