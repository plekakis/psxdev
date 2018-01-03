all: tuto3
	
tuto3:
	ccpsx -O3 -Xo$80040000 tuto3a.c -otuto3a.cpe,tuto3a.sym

for_menu:
	ccpsx -O3 -Xo$80040000 tuto3a.c \psx\lib\none2.obj -o tuto3a.cpe,tuto3a.sym
	cpe2x tuto3a.cpe
	del tuto3a.cpe

clean:
	del *.sym
	del *.cpe

