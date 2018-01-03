all:
	ccpsx -O2 -Xm -Xo$80010000 main.c -o main.cpe,main.sym,main.map

run:
	resetps 1
	copy ..\PDAsmpl\release\PDA.axf.ro pda.bin
	pqbload pda.bin 80100000
	delay
	run main
	testmess
