CFLAGS	= -O3
LDFLAGS	= -O3 -Xn1024
TADDR	= -Xo$80100000

all: common animview.cpe tuto0.cpe tuto1.cpe tuto2.cpe tuto3.cpe
	# EMPTY

common:
	cd ..\common
	psymake
	cd ..\anim

NONEOBJ	= ..\..\..\..\lib\none3.obj
DATDIR	= ..\..\..\..\data\hmd

CLIB	= ..\common\common.lib
OBJV	= animview.obj
OBJ0	= tuto0.obj
OBJ1	= tuto1.obj
OBJ2	= tuto2.obj
OBJ3	= tuto3.obj

.c.obj:
	ccpsx $(CFLAGS) -c -o $. $&.c

animview.cpe: common $(OBJV)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJV) -l$(CLIB)

tuto0.cpe: common $(OBJ0)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ0) -l$(CLIB)

tuto1.cpe: common $(OBJ1)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ1) -l$(CLIB)

tuto2.cpe: common $(OBJ2)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ2) -l$(CLIB)

tuto3.cpe: common $(OBJ3)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ3) -l$(CLIB)

for_menu: common $(OBJV) $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3)
	ccpsx $(LDFLAGS) $(TADDR) -oanimview.cpe $(OBJV) $(NONEOBJ) -l$(CLIB)
	cpe2x animview.cpe
	del animview.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto0.cpe $(OBJ0) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto0.cpe
	del tuto0.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto1.cpe $(OBJ1) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto1.cpe
	del tuto1.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto2.cpe $(OBJ2) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto2.cpe
	del tuto2.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto3.cpe $(OBJ3) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto3.cpe
	del tuto3.cpe

loadH:
	pqbload $(DATDIR)\scei\anim\balloon\balloon.hmd 80010000

load0:
	pqbload $(DATDIR)\scei\anim\tri3\tri.hmd 80010000

load1:
	pqbload $(DATDIR)\scei\anim\tri3\tri.hmd 80010000

load2:
	pqbload $(DATDIR)\scei\anim\cube0\cube0.hmd 80010000

load3:
	pqbload $(DATDIR)\scei\anim\tri4\tri.hmd 80010000
