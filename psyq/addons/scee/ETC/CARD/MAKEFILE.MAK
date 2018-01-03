CC = ccpsx
CFLAGS = -Xo$80010000 -Wall -g
PROGRAM =main

ALL = listcrd.cpe  writcrd.cpe

listcrd.cpe: listcrd.c
	$(CC) $(CFLAGS) -olistcrd.cpe,listcrd.sym listcrd.c

writcrd.cpe: writcrd.c asc2sjis.obj
	$(CC) $(CFLAGS) -owritcrd.cpe,writcrd.sym writcrd.c asc2sjis.obj

asc2sjis.obj: asc2sjis.c        
	$(CC) $(CFLAGS) -c -oasc2sjis.obj asc2sjis.c
