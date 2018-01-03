# If you link 2mbyte.obj instead of none3.obj, this program can start alone.

all:	balls balls2 exe
	
balls:
	ccpsx -O3 -Xo$80050000 main.c ..\..\..\..\lib\none3.obj -oballs.cpe,balls.sym,balls.map

balls2:
	ccpsx -O3 -Xo$80010000 -DDEAD_PARENT main.c ..\..\..\..\lib\none3.obj -oballs2.cpe,balls2.sym,balls2.map

exe:
	cpe2x balls.cpe
	cpe2x balls2.cpe
