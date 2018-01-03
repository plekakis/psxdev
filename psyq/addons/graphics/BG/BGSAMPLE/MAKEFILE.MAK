all:
	ccpsx -g -Xo$80080000 main.c -omain.cpe,main.sym

for_menu:
	ccpsx -g -Xo$80080000 \psx\lib\none2.obj main.c -obgsample.cpe
	cpe2x bgsample.cpe
	del   bgsample.cpe

load:
	pqbload sample.tim 80180000
	pqbload sample.bgd 80100000
	pqbload sample.cel 80140000
