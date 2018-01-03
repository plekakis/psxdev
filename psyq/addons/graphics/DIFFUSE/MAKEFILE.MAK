all:
	ccpsx -O -Xo$80010000 main.c -odiffuse.cpe,diffuse.sym
for_menu:
	ccpsx -O -Xo$80010000 main.c \psx\lib\none2.obj -odiffuse.cpe
	cpe2x diffuse.cpe
