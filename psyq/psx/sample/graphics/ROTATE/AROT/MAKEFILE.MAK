all:
	ccpsx -O -Xo$80010000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -O -Xo$80010000 main.c \psx\lib\none3.obj -oarot.cpe
	cpe2x arot.cpe
	del arot.cpe
