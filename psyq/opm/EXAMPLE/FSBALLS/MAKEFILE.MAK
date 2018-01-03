all:
        ccpsx -O3 -Xo$80018000 main.c -ofballs.cpe,fballs.sym

child:
      ccpsx -c -comments-c++ -O3 -Xo$80018000 main.c -ofballs.obj
		psylink @balls.lnk,fballs.cpe
		cpe2x /CA fballs.cpe	
