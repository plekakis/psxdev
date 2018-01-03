CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  =  -c -g -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = -Xo$80010000 /m
TEST_OBJS  = main.obj card.obj window.obj dongle.obj dongread.obj asc2sjis.obj datafile.obj

all  : main.cpe
       echo Done.

main.obj: main.c
	$(CC) $(CCOPTIONS) main.c -omain.obj

card.obj: card.c card.h
    $(CC) $(CCOPTIONS) card.c  -ocard.obj

window.obj: window.c window.h 
    $(CC) $(CCOPTIONS) window.c  -owindow.obj

dongle.obj: dongle.c dongle.h 
    $(CC) $(CCOPTIONS) dongle.c  -odongle.obj

dongread.obj: dongread.c dongread.h
    $(CC) $(CCOPTIONS) dongread.c  -odongread.obj

asc2sjis.obj: asc2sjis.c asc2sjis.h
    $(CC) $(CCOPTIONS) asc2sjis.c  -oasc2sjis.obj

datafile.obj: datafile.asm
    $(ASM) /l datafile.asm,datafile.obj
    

main.cpe: $(TEST_OBJS) 
    ccpsx -Xo$80010000 $(TEST_OBJS) -omain.cpe,main.sym,main.map

backup:
    copy *.c  *.d
