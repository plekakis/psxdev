all:
	ccpsx -O3 -Xo$80010000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O3 -Xo$80010000 main.c \psx\lib\none3.obj -oshadow.cpe
        cpe2x shadow.cpe
        del shadow.cpe
