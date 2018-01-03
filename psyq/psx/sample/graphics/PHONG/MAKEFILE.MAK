all:
	ccpsx -O -Xo$80010000 test.c scn.c  -omain.cpe,main.sym

for_menu:
        ccpsx -O -Xo$80010000 test.c scn.c \psx\lib\none3.obj -ophong.cpe
        cpe2x phong.cpe
        del phong.cpe


