STR = sample.str
main:
	ccpsx -O -Xo$80080000 main.c -omain.cpe,main.sym 
load:
	pqbload $(STR) a0200000

