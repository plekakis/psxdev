all:
	ccpsx -O4 -Xo$80010000 main.c -oballs.cpe,balls.sym

for_menu:
	ccpsx -O4 -Xo$80010000 main.c \psx\lib\none3.obj -ogetode.cpe
	cpe2x getode.cpe
	del getode.cpe
