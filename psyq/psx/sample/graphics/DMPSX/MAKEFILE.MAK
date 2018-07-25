all:
	ccpsx -O3 -c tuto0.c -otuto0.obj
	dmpsx tuto0.obj
	ccpsx -O3 -Xo$80010000 tuto0.obj -otuto0.cpe,tuto0.sym

for_menu:
	ccpsx -O3 -c tuto0.c -ogcube.obj
	dmpsx gcube.obj
	ccpsx -O3 -Xo$80010000 gcube.obj \psx\lib\none3.obj -ogcube.cpe
	cpe2x gcube.cpe
	del gcube.cpe
