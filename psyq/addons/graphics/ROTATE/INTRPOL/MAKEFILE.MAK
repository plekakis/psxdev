all: main
	
main:
	ccpsx -O -Xo$80040000 main.c -omain.cpe,main.sym,main.map


load:
	pqbload ../../data/tim/earth8.tim 80100000


for_menu: main_m
	
main_m:
	ccpsx -O -Xo$80040000 main.c c:\psx\lib\none2.obj -omain_m.cpe,main_m.sym,main_m.map
	cpe2x main_m.cpe
	del main_m.cpe
