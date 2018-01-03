DATDIR=..\..\..\data

all:
	ccpsx -O3 -c text.c text.obj
	dmpsx text.obj
	ccpsx -O3 -Xo$80010000 main.c divide.c text.obj -omain.cpe,main.sym

for_menu:
	ccpsx -O3 -c text.c text.obj
	dmpsx text.obj
	ccpsx -O3 -Xo$80010000 \psx\lib\none3.obj main.c divide.c text.obj \
	-ojimen.cpe,jimen.sym
	cpe2x jimen.cpe
	del jimen.cpe

load:
	pqbload $(DATDIR)\tim\stones.tim 801a0000
