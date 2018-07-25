all:
        ccpsx -O3 -Xo$80010000 main.c -oballs.cpe,balls.sym
for_menu:
	ccpsx -O3 -Xo$80010000 main.c \psx\lib\none2.obj -oballs.cpe
	cpe2x balls.cpe
