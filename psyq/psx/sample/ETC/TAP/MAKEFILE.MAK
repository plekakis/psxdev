all:
        ccpsx -O -Xo$80040000  main.c -omain.cpe,main.sym
for_menu:
        ccpsx -O -Xo$80040000  main.c \psx\lib\none3.obj -otap.cpe,main.sym
        cpe2x tap.cpe
        del tap.cpe
