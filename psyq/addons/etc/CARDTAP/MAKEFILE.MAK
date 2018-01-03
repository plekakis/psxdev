all:
	ccpsx -O -Xo$80040000 main.c cman2.c -otest.cpe,test.sym -llibtap.lib