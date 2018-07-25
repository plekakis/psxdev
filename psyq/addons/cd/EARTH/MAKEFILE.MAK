tuto0:
	ccpsx -O -Xo$80040000 tuto0.c cdadd.c db.c -oearth.cpe,earth.sym

for_menu:
	ccpsx -O -Xo$80040000 \psx\lib\none2.obj tuto0.c cdadd.c db.c -oearth.cpe
	cpe2x earth.cpe
	del   earth.cpe
load:
	pqbload earth.tmd   80120000



