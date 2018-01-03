all: view8 view15
	# EMPTY

view8:
	ccpsx -O3 -Xo$80100000 -DHALF main.c -oview8.cpe,view8.sym

view15:
	ccpsx -O3 -Xo$80100000 main.c -oview15.cpe,view15.sym

for_menu:
	ccpsx -O3 -Xo$80100000 -DHALF main.c \psx\lib\none3.obj -oview8.cpe,view8.sym
	cpe2x view8.cpe
	del view8.cpe
	ccpsx -O3 -Xo$80100000 main.c \psx\lib\none3.obj -oview15.cpe,view15.sym
	cpe2x view15.cpe
	del view15.cpe

loadkan:
	pqbload ..\fontdata\cxgb15g.dot 80010000
	pqbload ..\fontdata\codetbl.txt 80050000

loadhan:
	pqbload ..\fontdata\cxbig15g.dot 80010000
	pqbload ..\fontdata\codetbl.txt 80050000

loadhalf:
	pqbload ..\fontdata\hlf15g.dot 80010000

clean:
	del *.cpe
	del *.sym
	del *.exe
