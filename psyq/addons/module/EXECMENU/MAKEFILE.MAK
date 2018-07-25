OBJ = fonttex1.obj ..\..\..\lib\2mbyte.obj

all: $(OBJ)
	ccpsx -O3 -Xo$80010000 menu.c $(OBJ) -omenu.cpe,menu.sym,menu.map
	cpe2x menu.cpe
	del execmenu.exe
	ren menu.exe execmenu.exe

fonttex1.obj : fonttex1.c
	ccpsx -O3 -c fonttex1.c
