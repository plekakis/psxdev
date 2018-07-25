CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  =  -c -g -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj datafile.obj graphic.obj

all  : main.cpe
       echo Done.


main.obj: main.c main.h graphic.h
	$(CC) $(CCOPTIONS) main.c -omain.obj

datafile.obj: datafile.asm
 	asmpsx /l datafile.asm,datafile.obj

graphic.obj: graphic.c main.h	graphic.h
	$(CC) $(CCOPTIONS) graphic.c -ographic.obj


main.cpe: $(TEST_OBJS) main.lnk
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map


