all:
	ccpsx -O -Xo$80010000 main.c -otuto2.cpe,tuto2.sym
for_menu:
	ccpsx -O -Xo$80010000 main.c \psx\lib\none3.obj -otuto2.cpe,ctuto2.sym
	cpe2x tuto2.cpe
	del tuto2.cpe

