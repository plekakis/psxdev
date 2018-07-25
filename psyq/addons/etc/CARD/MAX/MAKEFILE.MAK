#
# file : makefile.mak
#
#	makefile for psymake
#
main.cpe: max.obj pad.obj emouse.obj vsync.obj drawmax.obj maxinit.obj menuset.obj card.obj
	psylink /c /o$80080000 @main.lnk,main.cpe,main.sym

for_menu: max.obj pad.obj emouse.obj vsync.obj drawmax.obj maxinit.obj menuset.obj card.obj
	psylink /c /o$80080000 @menu.lnk,main.cpe
	cpe2x main.cpe
	del main.cpe

max.obj: max.c
	ccpsx -c -O2  max.c

pad.obj: pad.c
	ccpsx -c -O2  pad.c

emouse.obj: emouse.c
	ccpsx -c -O2  emouse.c

vsync.obj: vsync.c
	ccpsx -c -O2  vsync.c

drawmax.obj: drawmax.c
	ccpsx -c -O2  drawmax.c

maxinit.obj: maxinit.c
	ccpsx -c -O2  maxinit.c

menuset.obj: menuset.c
	ccpsx -c -O2  menuset.c

card.obj: card.c
	ccpsx -c -O2  card.c

clean :
	del *.obj
	del *.cpe
	del *.sym
