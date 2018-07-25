CC = ccpsx -O3 -Xo$80010000

all: tuto1 tuto2 tuto3 tuto4 tuto5 tuto6 tuto7
	
mdec_bs.obj : mdec_bs.c
	ccpsx -c mdec_bs.c
tuto1:
	$(CC) tuto1.c -otuto1.cpe,tuto1.sym 
tuto2: 
	$(CC) tuto2.c -otuto2.cpe,tuto2.sym
tuto3: mdec_bs.obj 
	$(CC) tuto3.c mdec_bs.obj -otuto3.cpe,tuto3.sym 
tuto4: 
	$(CC) tuto4.c -otuto4.cpe,tuto4.sym
tuto5: 
	$(CC) tuto5.c -otuto5.cpe,tuto5.sym 
tuto6: mdec_bs.obj
	$(CC) tuto6.c mdec_bs.obj  -otuto6.cpe,tuto6.sym 
tuto7: mdec_bs.obj
	$(CC) tuto7.c mdec_bs.obj  -otuto7.cpe,tuto7.sym 

for_menu: mdec_bs.obj
	$(CC) \psx\lib\none2.obj tuto1.c             -otuto1.cpe
	$(CC) \psx\lib\none2.obj tuto2.c             -otuto2.cpe
	$(CC) \psx\lib\none2.obj tuto3.c mdec_bs.obj -otuto3.cpe
	$(CC) \psx\lib\none2.obj tuto4.c             -otuto4.cpe
	$(CC) \psx\lib\none2.obj tuto5.c             -otuto5.cpe
	$(CC) \psx\lib\none2.obj tuto6.c mdec_bs.obj -otuto6.cpe
	$(CC) \psx\lib\none2.obj tuto7.c mdec_bs.obj -otuto7.cpe
	cpe2x tuto1.cpe
	cpe2x tuto2.cpe
	cpe2x tuto3.cpe
	cpe2x tuto4.cpe
	cpe2x tuto5.cpe
	cpe2x tuto6.cpe
	cpe2x tuto7.cpe


