all: codeview.c
	ccpsx -O -Xo$80010000 codeview.c \psx\lib\2mbyte.obj -ocodeview.cpe,codeview.sym

for_menu: codeview.c
	ccpsx -O -Xo$80010000 codeview.c \psx\lib\none3.obj -ocodeview.cpe
	cpe2x codeview.cpe
	del codeview.cpe

clean:
	del codeview.cpe
	del codeview.sym
	del codeview.exe
