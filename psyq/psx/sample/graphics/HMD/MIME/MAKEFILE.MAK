CFLAGS	= -O3
LDFLAGS	= -O3 -Xn512
TADDR	= -Xo$80100000

all: common tuto0.cpe tuto1.cpe tuto2.cpe
	# EMPTY

common:
	cd ..\common
	psymake
	cd ..\mime

NONEOBJ	= ..\..\..\..\lib\none3.obj
DATDIR	= ..\..\..\..\data\hmd

CLIB	= ..\common\common.lib
OBJ0	= tuto0.obj control.obj
OBJ1	= tuto1.obj control.obj
OBJ2	= tuto2.obj control.obj

.c.obj:
	ccpsx $(CFLAGS) -c -o $. $&.c

MODELADDR = 80010000

tuto0.cpe: common $(OBJ0)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ0) -l$(CLIB)

tuto1.cpe: common $(OBJ1)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ1) -l$(CLIB)

tuto2.cpe: common $(OBJ2)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ2) -l$(CLIB)

for_menu: common $(OBJ0) $(OBJ1) $(OBJ2)
	ccpsx $(LDFLAGS) $(TADDR) -otuto0.cpe $(OBJ0) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto0.cpe
	del tuto0.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto1.cpe $(OBJ1) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto1.cpe
	del tuto1.cpe

	ccpsx $(LDFLAGS) $(TADDR) -otuto2.cpe $(OBJ2) $(NONEOBJ) -l$(CLIB)
	cpe2x tuto2.cpe
	del tuto2.cpe

load0:
	pqbload $(DATDIR)\scei\mime\triaxes.hmd $(MODELADDR)

load1:
	pqbload $(DATDIR)\scei\mime\puyo.hmd $(MODELADDR)

load2:
	pqbload $(DATDIR)\scei\mime\trirpy.hmd $(MODELADDR)

load3:
	pqbload $(DATDIR)\scei\mime\twinp\twinp.hmd $(MODELADDR)
