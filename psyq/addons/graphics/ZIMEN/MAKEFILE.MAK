CC = ccpsx -O3 -Xo$80010000
#CC = ccpsx -O -Xo$80010000

all: tuto0.cpe tuto1.cpe tuto2.cpe tuto3.cpe tuto4.cpe tuto5.cpe
	
libzimen.lib: clip.c bgtex.c db.c mesh.c geom.c pad.c debug.c map.c \
	      config.c div.c 
	$(CC) -c clip.c
	$(CC) -c bgtex.c
	$(CC) -c db.c
	$(CC) -c mesh.c
	$(CC) -c geom.c
	$(CC) -c pad.c
	$(CC) -c debug.c
	$(CC) -c map.c
	$(CC) -c config.c
	$(CC) -c div.c
	dmpsx div.obj
	del libzimen.lib
	psylib /a libzimen.lib \
		clip.obj bgtex.obj db.obj mesh.obj geom.obj \
		pad.obj debug.obj map.obj config.obj div.obj
none2.obj:
	copy \psx\lib\none2.obj .

tuto0.cpe: libzimen.lib
	$(CC) tuto0.c -llibzimen.lib -otuto0.cpe,tuto0.sym
tuto1.cpe: libzimen.lib 
	$(CC) -DVIEW_CLIP -DNO_DIV tuto1.c -llibzimen.lib -otuto1.cpe,tuto1.sym
tuto2.cpe: libzimen.lib
	$(CC) -DNO_DIV tuto1.c -llibzimen.lib -otuto2.cpe,tuto2.sym
tuto3.cpe: libzimen.lib
	$(CC) -DRAP_ROUND -DNO_DIV tuto1.c -llibzimen.lib -otuto3.cpe,tuto3.sym
tuto4.cpe: libzimen.lib
	$(CC) -DRAP_ROUND tuto1.c -llibzimen.lib -otuto4.cpe,tuto4.sym
tuto5.cpe: libzimen.lib
	$(CC) tuto5.c bg.c bgtex8.c -otuto5.cpe,tuto5.sym

for_menu: none2.obj libzimen.lib 
	$(CC) tuto0.c none2.obj -llibzimen.lib -otuto0.cpe
	$(CC) -DVIEW_CLIP -DNO_DIV tuto1.c none2.obj -llibzimen.lib -otuto1.cpe
	$(CC) -DNO_DIV tuto1.c none2.obj -llibzimen.lib -otuto2.cpe
	$(CC) -DRAP_ROUND -DNO_DIV tuto1.c none2.obj -llibzimen.lib -otuto3.cpe
	$(CC) -DRAP_ROUND tuto1.c none2.obj -llibzimen.lib -otuto4.cpe
	$(CC) tuto5.c bg.c bgtex8.c none2.obj -otuto5.cpe
	cpe2x tuto0.cpe
	cpe2x tuto1.cpe
	cpe2x tuto2.cpe
	cpe2x tuto3.cpe
	cpe2x tuto4.cpe
	cpe2x tuto5.cpe

