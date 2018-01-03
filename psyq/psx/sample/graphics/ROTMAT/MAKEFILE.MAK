CC_CPE	= ccpsx -O -G 0 -Xo$80010000
CC_EXE    = ccpsx -O -G 0 -Xo$80010000 \psx\lib\none3.obj

OBJS     = dif_rmat.c cube.c init.c

all:	dif_rmat
	
for_menu: a_dif_rm.exe
	
clean:
	del *.cpe
	del *.sym
	
dif_rmat:
	$(CC_CPE) $(OBJS) -odif_rmat.cpe,dif_rmat.sym
	
#
# followings are menu entry (not used in usually operation)
#
a_dif_rm.exe:
        $(CC_EXE) $(OBJS) -oa_dif_rm.cpe,a_dif_rm.sym
        cpe2x a_dif_rm.cpe
        del a_dif_rm.cpe
