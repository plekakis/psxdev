all:
	ccpsx -G 0 -O3 -Xm -Xo$801f0000 $(LIBRARY_PATH)\2mbyte.obj main.c -omain.cpe,main.sym,main.map

disp:
        ccpsx -DSTS_DISP -G 0 -O3 -Xm -Xo$801e0000 $(LIBRARY_PATH)\2mbyte.obj main.c -omain.cpe,main.sym,main.map
