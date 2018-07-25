CC = ccpsx
CFLAGS = -Xo$80010000
PROGRAM =main

SRC = main.c asc2sjis.c

all: $(SRC)
	$(CC) $(CFLAGS) -o$(PROGRAM).cpe,$(PROGRAM).sym $(SRC)

