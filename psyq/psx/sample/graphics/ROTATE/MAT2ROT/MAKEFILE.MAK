DATDIR=..\..\..\..\data

all: tuto0
	
tuto0:
	ccpsx -O -Xo$80040000 tuto0.c -otuto0.cpe,tuto0.sym,tuto0.map

load:
	pqbload $(DATDIR)\tim\earth8.tim 80100000


for_menu: tuto0_m
	
tuto0_m:
	ccpsx -O -Xo$80040000 tuto0.c c:\psx\lib\none3.obj -otuto0_m.cpe,tuto0_m.sym,tuto0_m.map
	cpe2x tuto0_m.cpe
	del tuto0_m.cpe
