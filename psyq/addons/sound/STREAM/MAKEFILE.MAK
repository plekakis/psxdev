all:	tuto1 tuto2
	

for_menu:	tuto1.exe tuto2.exe
	

tuto1:
	ccpsx -g -Xo$80010000 -Xm tuto1.c -otuto1.cpe,tuto1.sym,tuto1.map

tuto2:
	ccpsx -g -Xo$80010000 -Xm tuto2.c -otuto2.cpe,tuto2.sym,tuto2.map

tuto1.exe:
	ccpsx -g -Xo$80010000 -Xm -DFOR_MENU tuto1.c c:\psx\lib\none2.obj -otuto1.cpe,tuto1.sym
	cpe2x tuto1.cpe

tuto2.exe:
	ccpsx -g -Xo$80010000 -Xm -DFOR_MENU tuto2.c c:\psx\lib\none2.obj -otuto2.cpe,tuto2.sym
	cpe2x tuto2.cpe

load:
	pqbload tr1l_pad.vb 800a0000
	pqbload tr1r_pad.vb 8011a000
	pqbload tr2l_pad.vb 80194000
	pqbload tr2r_pad.vb 8020e000
	pqbload tr3l_pad.vb 80288000
	pqbload tr3r_pad.vb 80302000
	pqbload tr4l_pad.vb 8037c000
	pqbload tr4r_pad.vb 803f6000
	pqbload tr5l_pad.vb 80470000
	pqbload tr5r_pad.vb 804ea000
	pqbload tr6l_pad.vb 80564000
	pqbload tr6r_pad.vb 805de000
	pqbload tr7l_pad.vb 80658000
	pqbload tr7r_pad.vb 806d2000

load2:
	pqbload tr1l_pad.vb 800a0000
	pqbload tr1r_pad.vb 8011a000

load8:
	pqbload tr1l_pad.vb 800a0000
	pqbload tr1r_pad.vb 8011a000
	pqbload tr2l_pad.vb 80194000
	pqbload tr2r_pad.vb 8020e000
	pqbload tr3l_pad.vb 80288000
	pqbload tr3r_pad.vb 80302000
	pqbload tr4l_pad.vb 8037c000
	pqbload tr4r_pad.vb 803f6000

clean:
	del *.exe
	del *.sym
	del *.map
	del *.cpe
	del *.?~
	del *.??~
