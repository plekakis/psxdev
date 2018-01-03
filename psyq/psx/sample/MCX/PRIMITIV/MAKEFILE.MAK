all:
	ccpsx -O3 -Xo$80010000 main.c -o main.cpe,main.sym

for_menu:
	ccpsx -O3 -Xo$80010000 main.c \psx\lib\none3.obj -o main.cpe
	cpe2x main.cpe
