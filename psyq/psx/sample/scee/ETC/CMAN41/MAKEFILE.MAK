CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  = -c -g -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = -Xo$80010000 /m
TEST_OBJS  = cman40.obj memcard.obj datafile.obj

all  : cman40.cpe
       echo Done.

cman40.obj: cman40.c
        $(CC) $(CCOPTIONS) cman40.c -ocman40.obj

memcard.obj: memcard.c
    $(CC) $(CCOPTIONS) memcard.c  -omemcard.obj

datafile.obj: datafile.asm
    $(ASM) /l datafile.asm,datafile.obj

cman40.cpe: $(TEST_OBJS) 
    ccpsx -Xo$80010000 $(TEST_OBJS) -ocman40.cpe,cman40.sym,cman40.map -l libtap.lib

backup:
    copy *.c  *.d
