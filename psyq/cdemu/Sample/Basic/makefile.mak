CCC 	= ccpsx  -g -Xo$80080000
CC	= ccpsx -g -c 
LOAD = pqbload

OBJS =  cd.o main.o

main.exe:  $(OBJS)
        $(CCC) $(OBJS) -o main.cpe,main.sym
	cpe2x /ca main.cpe
	move main.exe psx.exe

main.o: main.c
        $(CC) main.c -o main.o

cd.o: cd.c
        $(CC) cd.c -o cd.o
