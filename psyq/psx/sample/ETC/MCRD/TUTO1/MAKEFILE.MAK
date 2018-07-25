all:
	ccpsx -O2 -G0 -Xo$80040000 main.c balls.c -omain.cpe,main.sym

for_menu:
	ccpsx -O2 -G0 -Xo$80040000 main.c balls.c \psx\lib\none3.obj -omain.cpe,main.sym
	cpe2x main.cpe
	del main.cpe
