# If you link 2mbyte.obj instead of none3.obj, this program can start alone.

all: rcube rcube2 exe

rcube:
	ccpsx -O3 -Xo$80050000 -DCDREAD main.c ..\..\..\..\lib\none3.obj -orcube.cpe,rcube.sym,rcube.map

rcube2:
	ccpsx -O3 -Xo$80010000 -DCDREAD -DDEAD_PARENT main.c ..\..\..\..\lib\none3.obj -orcube2.cpe,rcube2.sym,rcube2.map

exe:
	cpe2x rcube.cpe
	cpe2x rcube2.cpe
