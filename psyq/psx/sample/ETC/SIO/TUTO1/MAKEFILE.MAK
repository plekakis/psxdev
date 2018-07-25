all:
	ccpsx -O -Xo$80010000 main.c -otuto1.cpe,tuto1.sym
for_menu:
	ccpsx -O -Xo$80010000 main.c \psx\lib\none3.obj -otuto1.cpe,csio.sym
	cpe2x tuto1.cpe
	del tuto1.cpe

