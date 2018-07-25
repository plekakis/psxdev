all:
	ccpsx -O2 -G0 -Xo$80080000 main.c -o main.cpe,main.sym

for_menu:
	ccpsx -O2 -G0 -Xo$80080000 main.c \psx\lib\none3.obj -o main.cpe,main.sym
	cpe2x main.cpe
	del   main.cpe

clean:
	del *.obj
	del *.cpe
	del *.sym

