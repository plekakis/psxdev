
OBJS = ..\..\..\lib\2mbyte.obj menu.obj snd.obj fonttex1.obj balls.obj anim.obj rcube.obj address.obj

all: $(OBJS)
	psylink /n 512 /c /m @menu.lnk,menu.cpe,menu.sym,menu.map
	cpe2x menu.cpe
	del overmenu.exe
	ren menu.exe overmenu.exe

fonttex1.obj : fonttex1.c
	ccpsx -O3 -c fonttex1.c
menu.obj : menu.c
	ccpsx -O3 -c -DSOUND menu.c
snd.obj : snd.c
	ccpsx -O3 -c snd.c	

balls.obj : balls.c
	ccpsx -O3 -G0 -c -Wa,sballs -DOVERLAY balls.c	
# <-G0> 		not use global pointer(GP)
# <-Wa,s[name]>		define this module as group 'name'
rcube.obj : rcube.c
	ccpsx -O3 -G0 -c -Wa,srcube -DOVERLAY rcube.c
anim.obj : anim.c
	ccpsx -O3 -G0 -c -Wa,sanim -DOVERLAY anim.c	

address.obj: address.s
	asmpsx /l address,address

