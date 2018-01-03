all: tuto0
	
tuto0:
	ccpsx -O3 -Xo$80080000 tuto0.c cdadd.c -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -O3 -Xo$80080000 tuto0.c cdadd.c \psx\lib\none2.obj -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe

load:
	pqbload pat16.tim 80010000
	pqbload pat8.tim  80020000
	pqbload pat4.tim  80030000
	pqbload cube3.tmd 80040000
