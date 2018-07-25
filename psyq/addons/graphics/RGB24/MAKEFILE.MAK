all:
	ccpsx -O -Xo$80040000 main.c -orgb24.cpe,rgb24.sym
for_menu:
	ccpsx -O -Xo$80040000 main.c \psx\lib\none2.obj -orgb24.cpe
	cpe2x rgb24.cpe

