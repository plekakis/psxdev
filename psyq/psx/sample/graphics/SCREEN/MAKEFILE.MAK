all:
	ccpsx -O3 -Xo$80700000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -O3 -Xo$80010000 \psx\lib\none3.obj main.c -oscreen.cpe
	cpe2x screen.cpe
	del   screen.cpe
