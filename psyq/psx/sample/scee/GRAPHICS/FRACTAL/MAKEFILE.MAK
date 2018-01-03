#ASM  = asmpsx
CC   = ccpsx
LINK = psylink

CCOPTIONS  =  -c -O2 -mgpopt -G32 -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj

all  : main.cpe
       echo Done.

main.obj: fract.c
	$(CC) $(CCOPTIONS) fract.c -omain.obj

#main.cpe: $(TEST_OBJS) main.lnk
#	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map

main.cpe : $(TEST_OBJS)
	ccpsx -Xo$80010000 $(TEST_OBJS) -omain.cpe,main.sym,main.map 


