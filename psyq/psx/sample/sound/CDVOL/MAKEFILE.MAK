all:
	ccpsx -O -Xo$80010000 -Xm main.c -omain.cpe,main.sym

for_menu:
	ccpsx -O -Xo$80010000 -DFOR_MENU -Xm main.c c:\psx\lib\none3.obj -ocdvol.cpe,cdvol.sym
	cpe2x cdvol.cpe

clean:
	del *.exe
	del *.sym
	del *.map
	del *.cpe
	del *.?~
	del *.??~

load:
	
