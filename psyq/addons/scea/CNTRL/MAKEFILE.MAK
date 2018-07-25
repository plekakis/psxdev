all:
	ccpsx -g -comments-c++ -O -Xo$80040000 /m pad.c -opad.cpe,pad.sym,pad.map

        ccpsx -O -Xo$80040000 /m main.c -otest.cpe,main.sym,main.map
        ccpsx -O -Xo$80040000 /m main2.c gun_fnc.c -omain2.cpe,main2.sym,main2.map

