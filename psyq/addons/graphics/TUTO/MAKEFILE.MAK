CC_CPE    = ccpsx -O -G 0 -Xo$80010000 
CC_EXE    = ccpsx -O -G 0 -Xo$80010000 \psx\lib\none2.obj

OBJS0     = tuto0.c mat0tex.c mat1tex.c mat2tex.c mat3tex.c 
OBJS1     = tuto1.c balltex.c
OBJS2     = tuto2.c init.c
OBJS3     = tuto3.c cube.c init.c
OBJS4     = tuto4.c cube.c init.c
OBJS5     = tuto5.c cube.c init.c bgtex.c bgtex8.c
OBJS6     = tuto6.c cube.c init.c
OBJS7     = tuto7.c cube.c init.c
OBJS8     = tuto8.c wintex.c near0tex.c near1tex.c far0tex.c far1tex.c
OBJS9     = tuto9.c bgtex.c bgtex8.c
OBJS10    = tuto10.c init.c bgtex.c bgtex8.c
OBJS11    = tuto11.c init.c bgtex.c bgtex8.c
OBJS12    = tuto12.c lscr.c balls.c balltex.c
OBJS13    = tuto13.c balls.c balltex.c

all:	tuto0  tuto1  tuto2  tuto3 tuto4 tuto5  tuto6  tuto7  tuto8 tuto9  \
	tuto10 tuto11 tuto12 tuto13
	
for_menu: a_tuto0.exe  a_tuto1.exe  a_tuto2.exe  a_tuto3.exe a_tuto4.exe \
	  a_tuto5.exe  a_tuto6.exe  a_tuto7.exe  a_tuto8.exe a_tuto9.exe \
	  a_tuto10.exe a_tuto11.exe a_tuto12.exe a_tuto13.exe
	
clean:
	del *.cpe
	del *.sym
tuto0:
	$(CC_CPE) $(OBJS0) -otuto0.cpe,tuto0.sym
tuto1:
	$(CC_CPE) $(OBJS1) -otuto1.cpe,tuto1.sym
tuto2:
	$(CC_CPE) $(OBJS2) -otuto2.cpe,tuto2.sym
tuto3:
	$(CC_CPE) $(OBJS3) -otuto3.cpe,tuto3.sym
tuto4:
	$(CC_CPE) $(OBJS4) -otuto4.cpe,tuto4.sym
tuto5:
	$(CC_CPE) $(OBJS5) -otuto5.cpe,tuto5.sym
tuto6:
	$(CC_CPE) $(OBJS6) -otuto6.cpe,tuto6.sym
tuto7:
	$(CC_CPE) $(OBJS7) -otuto7.cpe,tuto7.sym
tuto8:
	$(CC_CPE) $(OBJS8) -otuto8.cpe,tuto8.sym
tuto9:
	$(CC_CPE) $(OBJS9) -otuto9.cpe,tuto9.sym
tuto10:
	$(CC_CPE) $(OBJS10) -otuto10.cpe,tuto10.sym
tuto11:
	$(CC_CPE) $(OBJS11) -otuto11.cpe,tuto13.sym
tuto12:
	$(CC_CPE) $(OBJS12) -otuto12.cpe,tuto12.sym
tuto13:
	$(CC_CPE) $(OBJS13) -otuto13.cpe,tuto13.sym
#
# followings are menu entry (not used in usually operation)
#
a_tuto0.exe:
	$(CC_EXE) $(OBJS0) -oa_tuto0.cpe,a_tuto0.sym
	cpe2x a_tuto0.cpe
a_tuto1.exe:
	$(CC_EXE) $(OBJS1) -oa_tuto1.cpe,a_tuto1.sym
	cpe2x a_tuto1.cpe
a_tuto2.exe:
	$(CC_EXE) $(OBJS2) -oa_tuto2.cpe,a_tuto2.sym
	cpe2x a_tuto2.cpe
a_tuto3.exe:
	$(CC_EXE) $(OBJS3) -oa_tuto3.cpe,a_tuto3.sym
	cpe2x a_tuto3.cpe
a_tuto4.exe:
	$(CC_EXE) $(OBJS4) -oa_tuto4.cpe,a_tuto4.sym
	cpe2x a_tuto4.cpe
a_tuto5.exe:
	$(CC_EXE) $(OBJS5) -oa_tuto5.cpe,a_tuto5.sym
	cpe2x a_tuto5.cpe
a_tuto6.exe:
	$(CC_EXE) $(OBJS6) -oa_tuto6.cpe,a_tuto6.sym
	cpe2x a_tuto6.cpe
a_tuto7.exe:
	$(CC_EXE) $(OBJS7) -oa_tuto7.cpe,a_tuto7.sym
	cpe2x a_tuto7.cpe
a_tuto8.exe:
	$(CC_EXE) $(OBJS8) -oa_tuto8.cpe,a_tuto8.sym
	cpe2x a_tuto8.cpe
a_tuto9.exe:
	$(CC_EXE) $(OBJS9) -oa_tuto9.cpe,a_tuto9.sym
	cpe2x a_tuto9.cpe
a_tuto10.exe:
	$(CC_EXE) $(OBJS10) -oa_tuto10.cpe,a_tuto10.sym
	cpe2x a_tuto10.cpe
a_tuto11.exe:
	$(CC_EXE) $(OBJS11) -oa_tuto11.cpe,a_tuto13.sym
	cpe2x a_tuto11.cpe
a_tuto12.exe:
	$(CC_EXE) $(OBJS12) -oa_tuto12.cpe,a_tuto12.sym
	cpe2x a_tuto12.cpe
a_tuto13.exe:
	$(CC_EXE) $(OBJS13) -oa_tuto13.cpe,a_tuto13.sym
	cpe2x a_tuto13.cpe


