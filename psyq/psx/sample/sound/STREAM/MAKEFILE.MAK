DATDIR=..\..\..\data

all:	tuto1 tuto2
	

for_menu:	tuto1.exe tuto2.exe
	

tuto1:
	ccpsx -g -Xo$80010000 -Xm tuto1.c -otuto1.cpe,tuto1.sym,tuto1.map

tuto2:
	ccpsx -g -Xo$80010000 -Xm tuto2.c -otuto2.cpe,tuto2.sym,tuto2.map

tuto1.exe:
	ccpsx -g -Xo$80010000 -Xm -DFOR_MENU tuto1.c c:\psx\lib\none3.obj -otuto1.cpe,tuto1.sym
	cpe2x tuto1.cpe

tuto2.exe:
	ccpsx -g -Xo$80010000 -Xm -DFOR_MENU tuto2.c c:\psx\lib\none3.obj -otuto2.cpe,tuto2.sym
	cpe2x tuto2.cpe

load:
	pqbload $(DATDIR)\vab\tr1l_pad.vb 800a0000
	pqbload $(DATDIR)\vab\tr1r_pad.vb 8011a000
	pqbload $(DATDIR)\vab\tr2l_pad.vb 80194000
	pqbload $(DATDIR)\vab\tr2r_pad.vb 8020e000
	pqbload $(DATDIR)\vab\tr3l_pad.vb 80288000
	pqbload $(DATDIR)\vab\tr3r_pad.vb 80302000
	pqbload $(DATDIR)\vab\tr4l_pad.vb 8037c000
	pqbload $(DATDIR)\vab\tr4r_pad.vb 803f6000
	pqbload $(DATDIR)\vab\tr5l_pad.vb 80470000
	pqbload $(DATDIR)\vab\tr5r_pad.vb 804ea000
	pqbload $(DATDIR)\vab\tr6l_pad.vb 80564000
	pqbload $(DATDIR)\vab\tr6r_pad.vb 805de000
	pqbload $(DATDIR)\vab\tr7l_pad.vb 80658000
	pqbload $(DATDIR)\vab\tr7r_pad.vb 806d2000

load2:
	pqbload $(DATDIR)\vab\tr1l_pad.vb 800a0000
	pqbload $(DATDIR)\vab\tr1r_pad.vb 8011a000

loadapd:
	pqbload $(DATDIR)\vab\tr1l_pad.vb 800c0000
	pqbload $(DATDIR)\vab\tr1r_pad.vb 8013a000
	pqbload $(DATDIR)\vab\tr2l_pad.vb 801b4000
	pqbload $(DATDIR)\vab\tr2r_pad.vb 8022e000
	pqbload $(DATDIR)\vab\tr3l_pad.vb 802a8000
	pqbload $(DATDIR)\vab\tr3r_pad.vb 80322000
	pqbload $(DATDIR)\vab\tr4l_pad.vb 8039c000
	pqbload $(DATDIR)\vab\tr4r_pad.vb 80416000

load8:
	pqbload $(DATDIR)\vab\tr1l_pad.vb 800a0000
	pqbload $(DATDIR)\vab\tr1r_pad.vb 8011a000
	pqbload $(DATDIR)\vab\tr2l_pad.vb 80194000
	pqbload $(DATDIR)\vab\tr2r_pad.vb 8020e000
	pqbload $(DATDIR)\vab\tr3l_pad.vb 80288000
	pqbload $(DATDIR)\vab\tr3r_pad.vb 80302000
	pqbload $(DATDIR)\vab\tr4l_pad.vb 8037c000
	pqbload $(DATDIR)\vab\tr4r_pad.vb 803f6000

clean:
	del *.exe
	del *.sym
	del *.map
	del *.cpe
	del *.?~
	del *.??~
