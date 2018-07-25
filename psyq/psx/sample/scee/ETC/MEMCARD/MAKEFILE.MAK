CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  = -c -comments-c++ 
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = -Xo$80010000
TEST_OBJS  = main.obj datafile.obj

all  : main.cpe
       echo Done.

main.obj: main.c
	$(CC) $(CCOPTIONS) main.c -omain.obj

datafile.obj: datafile.asm
    $(ASM) /l datafile.asm,datafile.obj
    

main.cpe: $(TEST_OBJS) 
    ccpsx -Xo$80010000 $(TEST_OBJS) -omain.cpe,main.sym,main.map -l libtap.lib -l memcard.lib

backup:
    copy *.c  *.d
                              
