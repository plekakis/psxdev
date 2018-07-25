# If you link 2mbyte.obj instead of none3.obj, this program can start alone.

all: anim anim2 exe
	
anim:
	ccpsx -O3 -Xo$80050000 main.c ..\..\..\..\lib\none3.obj -oanim.cpe,anim.sym,anim.map
	
anim2:
	ccpsx -O3 -Xo$80010000 -DDEAD_PARENT main.c ..\..\..\..\lib\none3.obj -oanim2.cpe,anim2.sym,anim2.map

exe:
	cpe2x anim.cpe
	cpe2x anim2.cpe
