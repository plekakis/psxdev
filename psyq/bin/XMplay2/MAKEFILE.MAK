#ASM  = asmpsx
CC   = ccpsx
LINK = psylink

CCOPTIONS  =  -Wall -c -O2 -mgpopt -G0 -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj graph.obj datafile.obj
all  : main.cpe
       echo Done.

main.obj: main.c
	$(CC) $(CCOPTIONS) main.c -omain.obj

datafile.obj: datafile.asm
        asmpsx /l datafile.asm,datafile.obj

main.cpe: $(TEST_OBJS) main.lnk
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map

