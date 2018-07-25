all:
	ccpsx -O -Xo$80010000 main.c todanim.c objctl.c -omain.cpe,main.sym

for_menu:
	ccpsx -O -Xo$80010000 \psx\lib\none2.obj main.c todanim.c objctl.c -ojanken.cpe
	cpe2x janken.cpe
	del   janken.cpe

load:
	pqbload tmd\te.tmd      800c0000
	pqbload tod\appear.tod  800e0000
	pqbload tod\jaanken.tod 80100000
	pqbload tod\n_g.tod     80120000
	pqbload tod\n_c.tod     80140000
	pqbload tod\n_p.tod     80160000
	pqbload tod\g_n.tod     80180000
	pqbload tod\c_n.tod     801a0000
	pqbload tod\p_n.tod     801c0000
	pqbload tod\disapp.tod  801e0000
