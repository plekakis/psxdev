all: tuto0 tuto1 tuto2 tuto3
	
tuto0:
	ccpsx -O3 -Xo$80040000 tuto0.c -otuto0.cpe,tuto0.sym

tuto1:
	ccpsx -O3 -Xo$80040000 tuto1.c -otuto1.cpe,tuto1.sym

tuto2:
	ccpsx -O3 -Xo$80040000 tuto2.c -otuto2.cpe,tuto2.sym

tuto3:
	ccpsx -O3 -Xo$80040000 tuto3.c -otuto3.cpe,tuto3.sym

for_menu:
	ccpsx -O3 -Xo$80040000 tuto0.c \psx\lib\none3.obj -o tuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
	del tuto0.cpe
	
	ccpsx -O3 -Xo$80040000 tuto1.c \psx\lib\none3.obj -o tuto1.cpe,tuto1.sym
	cpe2x tuto1.cpe
	del tuto1.cpe

	ccpsx -O3 -Xo$80040000 tuto2.c \psx\lib\none3.obj -o tuto2.cpe,tuto2.sym
	cpe2x tuto2.cpe
	del tuto2.cpe

	ccpsx -O3 -Xo$80040000 tuto3.c \psx\lib\none3.obj -o tuto3.cpe,tuto3.sym
	cpe2x tuto3.cpe
	del tuto3.cpe

clean:
	del *.sym
	del *.cpe

