all:
        ccpsx -c -O3 -Xo$80018000 -omenu.obj  main.c 

child:
        ccpsx -c -comments-c++ -DLINKED_STARTUP -O3 -Xo$80018000 -omenu.obj  main.c
		psylink @menu.lnk,menu.cpe
		cpe2x /CA menu.cpe
asm:
        ccpsx -O3 -S -Xo$80018000 main.c     

