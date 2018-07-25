all: tuto0 tuto1
	
tuto0:
	ccpsx -O3 -Xo$80040000 tuto0.c bgtex.c -otuto0.cpe,tuto0.sym
tuto1:
	ccpsx -O3 -Xo$80040000 tuto1.c bgtex.c -otuto1.cpe,tuto1.sym


test:
	ccpsx -O3 -Xo$80040000 -DAPD_SAVE tuto0.c bgtex.c \
	\psx\lib\autopad.obj -oqmesh0s.cpe
#	..\..\..\..\lib\autopad.obj -oqmesh0s.cpe

for_menu: qmesh0 qmesh1
	
qmesh0:
	ccpsx -O -Xo$80040000 tuto0.c bgtex.c c:\psx\lib\none3.obj -oqmesh0.cpe,qmesh0.sym,qmesh0.map
	cpe2x qmesh0.cpe
	del qmesh0.cpe

qmesh1:
	ccpsx -O -Xo$80040000 tuto1.c bgtex.c c:\psx\lib\none3.obj -oqmesh1.cpe,qmesh1.sym,qmesh1.map
	cpe2x qmesh1.cpe
	del qmesh1.cpe
