CC = ccpsx
OPTIONS = -g -Xo$80010000
OPT2 = -O2 -Xo$80010000

  
multitap.cpe: multitap.c
	$(CC) $(OPTIONS) multitap.c -omultitap.cpe,multitap.sym -l libtap.lib

