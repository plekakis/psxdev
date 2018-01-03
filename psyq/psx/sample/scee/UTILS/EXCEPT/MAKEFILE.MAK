#	Makefile for Exception Handler

test.cpe: vsdie.obj test.obj vsex.obj
	psylink @test.lnk,test.cpe,test.sym,test.map
	
vsdie.obj: vsdie.c
	ccpsx -g -c vsdie.c

test.obj : test.c
	ccpsx -g -c test.c

vsex.obj : vsex.s
	asmpsx /l /g vsex.s, vsex.obj
