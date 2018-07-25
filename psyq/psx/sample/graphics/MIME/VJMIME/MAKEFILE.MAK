DATDIR=..\..\..\..\data
TADDR	 =80010000

SRCS0 =main.c control.c matrix.c preset.c ctlfc.c rpy_intr.c a_intr.c tuto0.c nmime.c
SRCS1 =main.c control.c matrix.c preset.c ctlfc.c rpy_intr.c a_intr.c tuto1.c

all: tuto0 tuto1
	

tuto0: 
	ccpsx -O2 -Xo$$(TADDR) $(SRCS0) -otuto0.cpe,tuto0.sym

tuto1: 
	ccpsx -O2 -Xo$$(TADDR) $(SRCS1) -otuto1.cpe,tuto1.sym


load0:
	pqbload $(DATDIR)\tmd\base.tmd 80100000
	pqbload base.vdf 80108000

load1:
	pqbload $(DATDIR)\tmd\base.tmd 80100000

for_menu: tuto0_m tuto1_m
	

tuto0_m:
	echo $(SRCS0) > srcs.tmp
	ccpsx -O2 -Xo$80040000 @srcs.tmp c:\psx\lib\none3.obj -otuto0_m.cpe
	cpe2x tuto0_m.cpe
	del srcs.tmp
	del tuto0_m.cpe

tuto1_m:
	echo $(SRCS1) > srcs.tmp
	ccpsx -O2 -Xo$80040000 @srcs.tmp c:\psx\lib\none3.obj -otuto1_m.cpe
	cpe2x tuto1_m.cpe
	del srcs.tmp
	del tuto1_m.cpe
