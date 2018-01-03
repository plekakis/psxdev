SRCS = clip.c db.c mesh.c geom.c pad.c debug.c map.c \
	config.c div.c bgtex.c bgtex8.c mudtex.c
OBJS = clip.obj db.obj mesh.obj geom.obj pad.obj debug.obj \
	map.obj config.obj div.obj bgtex.obj bgtex8.obj mudtex.obj
OBJS1 = tuto1.obj tuto2.obj tuto3.obj tuto4.obj
OBJS2 = main.c tuto0.c tuto5.c bg.c

main : zimen.lib
	ccpsx -O3 -Xo$80020000 $(OBJS2) -lzimen.lib -omain.cpe,main.sym

for_menu: zimen.lib
	ccpsx -O3 -Xo$80020000 $(OBJS2) \psx\lib\none3.obj -lzimen.lib -omain.cpe
	cpe2x main.cpe
	del main.cpe

zimen.lib:
	ccpsx -O3 -c $(SRCS)
	dmpsx div.obj
	del zimen.lib
	psylib /a zimen.lib $(OBJS)
	ccpsx -O3 -c -DFNAME=tuto1 -DVIEW_CLIP -DNO_DIV tuto1.c -o tuto1.obj
	ccpsx -O3 -c -DFNAME=tuto2 -DNO_DIV tuto1.c -o tuto2.obj
	ccpsx -O3 -c -DFNAME=tuto3 -DRAP_ROUND -DNO_DIV tuto1.c -o tuto3.obj
	ccpsx -O3 -c -DFNAME=tuto4 -DRAP_ROUND tuto1.c -o tuto4.obj 
	psylib /a zimen.lib $(OBJS1)

clean:
	del *.lib
	del *.obj
	del *.cpe
	del *.sym
	del *.exe
