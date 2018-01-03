
tree:
	ccpsx -O3 -Xo$80010000 main.c ../../../lib/2mbyte.obj -omain.cpe,main.sym

for_menu:
	ccpsx -O3 -Xo$80010000 \psx\lib\none3.obj main.c -otree.cpe
	cpe2x tree.cpe
	del   tree.cpe
