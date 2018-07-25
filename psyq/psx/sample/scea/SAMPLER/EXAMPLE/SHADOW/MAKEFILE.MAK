all:
	ccpsx -O3 -Xo$80010000 main.c -omain.cpe,main.sym

child:
   ccpsx -c -comments-c++ -O3 -Xo$80018000 main.c -oshadow.obj
	psylink @shadow.lnk,shadow.cpe
	cpe2x /CA shadow.cpe
