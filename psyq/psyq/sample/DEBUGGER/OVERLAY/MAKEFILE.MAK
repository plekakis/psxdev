
main.cpe: main.obj level1.obj level2.obj level3.obj address.obj
	psylink /v /c /m @main.lnk,main.cpe,main.sym,main.map

main.obj: main.c common.h makefile.mak
	ccpsx -c -g main.c

level1.obj: level1.c common.h makefile.mak
	ccpsx -c -g -G0 -Wa,sl1 level1.c

level2.obj: level2.c common.h makefile.mak
	ccpsx -c -g -G0 -Wa,sl2 level2.c

level3.obj: level3.c common.h makefile.mak
	ccpsx -c -g -G0 -Wa,sl3 level3.c

address.obj: address.s makefile.mak
	asmpsx /l address,address

