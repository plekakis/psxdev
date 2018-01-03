CFLAGS	= -O3
LDFLAGS	= -O3 -Xn512
TADDR	= -Xo$80100000

all: common tuto0.cpe tuto1.cpe tuto2.cpe tuto3.cpe normview.cpe
	# EMPTY

common:
	cd ..\common
	psymake
	cd ..\basic

NONEOBJ	= ..\..\..\..\lib\none3.obj
DATDIR	= ..\..\..\..\data\hmd
	
CLIB	= ..\common\common.lib
OBJN	= normview.obj
OBJ0	= tuto0.obj
OBJ1	= tuto1.obj
OBJ2	= tuto2.obj
OBJ3	= tuto3.obj

.c.obj:
	ccpsx $(CFLAGS) -c -o $. $&.c

normview.cpe: common
	ccpsx $(CFLAGS) -DNORMVIEW -c -o $(OBJN) tuto0.c
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJN) -l$(CLIB)

tuto0.cpe: common $(OBJ0)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ0) -l$(CLIB)

tuto1.cpe: common $(OBJ1)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ1) -l$(CLIB)

tuto2.cpe: common $(OBJ2)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ2) -l$(CLIB)

tuto3.cpe: common $(OBJ3)
	ccpsx $(LDFLAGS) $(TADDR) -o$. $(OBJ3) -l$(CLIB)

for_menu: common $(OBJ0) $(OBJ1) $(OBJ2) $(OBJ3)
	ccpsx $(CFLAGS) -DNORMVIEW -c -o $(OBJN) tuto0.c
	ccpsx $(LDFLAGS) $(TADDR) -onormview.cpe $(OBJN) $(NONEOBJ) -l$(CLIB)
	cpe2x normview.cpe
	del normview.cpe

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

load0:
	pqbload $(DATDIR)\scei\basic\tri.hmd 80010000
#	pqbload $(DATDIR)\scei\basic\dino.hmd 80010000
#	pqbload $(DATDIR)\scei\basic\cube.hmd 80010000
#	pqbload $(DATDIR)\scei\basic\tile.hmd 80010000
#	pqbload $(DATDIR)\scei\basic\ground.hmd 80010000

load1:
	pqbload $(DATDIR)\scei\basic\flag.hmd 80010000

load2:
	pqbload $(DATDIR)\scei\basic\shuttle.hmd 80010000

load3:
	pqbload $(DATDIR)\scei\basic\jimen.hmd 80010000
