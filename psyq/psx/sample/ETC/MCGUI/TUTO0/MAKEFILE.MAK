DATDIR=..\..\..\..\data

all:
	ccpsx -O2 -G0 -Xo$80080000 main.c balls.c \psx\lib\mcgui.obj -o main.cpe,main.sym

for_menu:
	ccpsx -O2 -G0 -Xo$80080000 main.c balls.c \psx\lib\mcgui.obj -o main.cpe,main.sym
	cpe2x main.cpe
	del   main.cpe

clean:
	del *.obj
	del *.cpe
	del *.sym

load:
	pqbload $(DATDIR)\tim\mcgui.tim 80100000
	pqbload $(DATDIR)\seq\mcgui.seq 80104220
	pqbload $(DATDIR)\vab\mcgui_sq.vh  80108000
	pqbload $(DATDIR)\vab\mcgui_sq.vb  80109620
	pqbload $(DATDIR)\vab\mcgui_se.vh  80118000
	pqbload $(DATDIR)\vab\mcgui_se.vb  80118c20
	pqbload $(DATDIR)\tim\mcgui_bg.tim  80120000
