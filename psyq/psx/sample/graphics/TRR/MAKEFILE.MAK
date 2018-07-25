all:
	ccpsx -O3 -Xo$80010000 main.c -omain.cpe,main.sym

for_menu:
        ccpsx -O3 -Xo$80010000 main.c \psx\lib\none3.obj -otrr.cpe
        cpe2x trr.cpe
        del trr.cpe

