all:
	ccpsx -O -Xo$80040000 main.c mouse.c -otest.cpe,test.sym 
for_menu:
	ccpsx -O -Xo$80010000 main.c mouse.c \psx\lib\none3.obj -omouse.cpe,mouse.sym 
	cpe2x mouse.cpe
	del mouse.cpe
