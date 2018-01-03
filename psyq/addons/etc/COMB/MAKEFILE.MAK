all:
	ccpsx -G0 -O3 -Xo$80010000  main8.c rc9.c -omain.cpe,main.sym
for_menu:
	ccpsx -O -Xo$80010000 main8.c rc9.c \psx\lib\none2.obj -omain.cpe,main.sym
	cpe2x main.cpe
	del main.cpe

