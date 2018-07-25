all:
	ccpsx -O -Xo$80010000 main.c effect.c mesh.c glasstex.c \
	-o2d.cpe,2d.sym

for_menu:
	ccpsx -O -Xo$80010000 main.c effect.c mesh.c glasstex.c \
	\psx\lib\none2.obj -o2d.cpe
	cpe2x 2d.cpe
