
OBJS = ..\..\..\lib\2mbyte.obj main.obj address.obj
OBJS2= \psx\lib\none3.obj main.obj address.obj
OBJSS= \psx\lib\autopad.obj mains.obj address.obj
OBJSL= \psx\lib\autopad.obj mainl.obj address.obj

all: $(OBJS)
	psylink /c /m @main.lnk,main.cpe,main.sym,main.map

for_menu: $(OBJS2)
	psylink /c /m @scratch.lnk,scratch.cpe
	cpe2x scratch.cpe
	del   scratch.cpe

main.obj : main.c
	ccpsx -O0 -c main.c
mains.obj : main.c
	ccpsx -O0 -DAPD_SAVE -c main.c -omains.obj
mainl.obj : main.c
	ccpsx -O0 -DAPD_LOAD -c main.c -omainl.obj

#You can change degree of optimization and test it.

address.obj: address.s
	asmpsx /l address,address
