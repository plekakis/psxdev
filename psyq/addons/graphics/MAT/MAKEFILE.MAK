all:
	ccpsx -g -Xo$80010000 main.c mat0tex.c mat1tex.c mat2tex.c mat3tex.c \
							-omat.cpe,mat.sym
for_menu:
	ccpsx -O -Xo$80010000 main.c \
	mat0tex.c mat1tex.c mat2tex.c mat3tex.c \psx\lib\none2.obj -o mat.cpe
	cpe2x mat.cpe

