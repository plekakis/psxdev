DATDIR=..\..\..\..\data

all: tuto0 tuto1 tuto2
	
tuto0:	
	ccpsx -O3 -Xo$80010000 tuto0.c -otuto0.cpe,tuto0.sym
tuto1:	
	ccpsx -O3 -Xo$80010000 tuto1.c -otuto1.cpe,tuto1.sym
tuto2:
	ccpsx -O3 -c tuto2.c -otuto2.obj
	dmpsx tuto2.obj
	ccpsx -O3 -Xo$80010000 tuto2.obj -otuto2.cpe,tuto2.sym

for_menu: smesh0 smesh1 smesh2
	
smesh0:
        ccpsx -O3 -Xo$80010000 tuto0.c c:\psx\lib\none3.obj -osmesh0.cpe,smesh0.sym,smesh0.map
        cpe2x smesh0.cpe
        del smesh0.cpe

smesh1:
        ccpsx -O3 -Xo$80010000 tuto1.c c:\psx\lib\none3.obj -osmesh1.cpe,smesh1.sym,smesh1.map
        cpe2x smesh1.cpe
        del smesh1.cpe

smesh2:
	ccpsx -O3 -c tuto2.c -osmesh2.obj
	dmpsx smesh2.obj
	ccpsx -O3 -Xo$80010000 smesh2.obj c:\psx\lib\none3.obj -osmesh2.cpe,smesh2.sym,smesh2.map
        cpe2x smesh2.cpe
        del smesh2.cpe

load:
	pqbload $(DATDIR)\tim\earth4.tim 80070000
load1:
	pqbload $(DATDIR)\tim\rcube.tim 80070000
load2:
	pqbload $(DATDIR)\tim\rcube.tim 80120000

