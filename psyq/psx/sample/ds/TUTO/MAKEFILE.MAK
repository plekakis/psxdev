CC_CPE	= ccpsx -O -G 0 -Xo$80010000
CC_EXE	= ccpsx -O -G 0 -Xo$80010000 \psx\lib\none3.obj

OBJS0	= tuto0.c balls.c menu.c
OBJS1	= tuto1.c balls.c menu.c
OBJS2	= tuto2.c balls.c menu.c repeat.c
OBJS3	= tuto3.c balls.c menu.c
OBJS4	= tuto4.c balls.c menu.c
OBJS5	= tuto5.c balls.c menu.c repeat.c
OBJS6	= tuto6.c balls.c menu.c repeat.c
OBJS7	= tuto7.c balls.c sound.c sdata.c
OBJS8	= tuto8.c chain.c bg.c boytex.c sound.c sdata.c
OBJS9	= tuto9.c bg.c boytex.c sound.c sdata.c menu.c
OBJS10	= tuto10.c menu.c
OBJS11	= tuto11.c
OBJS12	= tuto12.c balls.c
OBJS13	= tuto13.c balls.c
OBJS14	= tuto14.c balls.c

all:	tuto0 tuto1 tuto2 tuto3 tuto4 \
	tuto5 tuto6 tuto7 tuto8 tuto9 \
	tuto10 tuto11 tuto12 tuto13 tuto14
	
for_menu: tuto0.exe tuto1.exe tuto2.exe tuto3.exe tuto4.exe \
	tuto5.exe tuto6.exe tuto7.exe tuto8.exe tuto9.exe \
	tuto10.exe tuto11.exe tuto12.exe tuto13.exe tuto14.exe
	

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
	$(CC_CPE) $(OBJS11) -otuto11.cpe,tuto11.sym
tuto12:
	$(CC_CPE) $(OBJS12) -otuto12.cpe,tuto12.sym
tuto13:
	$(CC_CPE) $(OBJS13) -otuto13.cpe,tuto13.sym
tuto14:
	$(CC_CPE) $(OBJS14) -otuto14.cpe,tuto14.sym

#
# followings are menu entry (not used in usual operations)
#
tuto0.exe:
	$(CC_EXE) $(OBJS0) -otuto0.cpe,tuto0.sym
	cpe2x tuto0.cpe
tuto1.exe:
	$(CC_EXE) $(OBJS1) -otuto1.cpe,tuto1.sym
	cpe2x tuto1.cpe
tuto2.exe:
	$(CC_EXE) $(OBJS2) -otuto2.cpe,tuto2.sym
	cpe2x tuto2.cpe
tuto3.exe:
	$(CC_EXE) $(OBJS3) -otuto3.cpe,tuto3.sym
	cpe2x tuto3.cpe
tuto4.exe:
	$(CC_EXE) $(OBJS4) -otuto4.cpe,tuto4.sym
	cpe2x tuto4.cpe
tuto5.exe:
	$(CC_EXE) $(OBJS5) -otuto5.cpe,tuto5.sym
	cpe2x tuto5.cpe
tuto6.exe:
	$(CC_EXE) $(OBJS6) -otuto6.cpe,tuto6.sym
	cpe2x tuto6.cpe
tuto7.exe:
	$(CC_EXE) $(OBJS7) -otuto7.cpe,tuto7.sym
	cpe2x tuto7.cpe
tuto8.exe:
	$(CC_EXE) $(OBJS8) -otuto8.cpe,tuto8.sym
	cpe2x tuto8.cpe
tuto9.exe:
	$(CC_EXE) $(OBJS9) -otuto9.cpe,tuto9.sym
	cpe2x tuto9.cpe
tuto10.exe:
	$(CC_EXE) $(OBJS10) -otuto10.cpe,tuto10.sym
	cpe2x tuto10.cpe
tuto11.exe:
	$(CC_EXE) $(OBJS11) -otuto11.cpe,tuto11.sym
	cpe2x tuto11.cpe
tuto12.exe:
	$(CC_EXE) $(OBJS12) -otuto12.cpe,tuto12.sym
	cpe2x tuto12.cpe
tuto13.exe:
	$(CC_EXE) $(OBJS13) -otuto13.cpe,tuto13.sym
	cpe2x tuto13.cpe
tuto14.exe:
	$(CC_EXE) $(OBJS14) -otuto14.cpe,tuto14.sym
	cpe2x tuto14.cpe
