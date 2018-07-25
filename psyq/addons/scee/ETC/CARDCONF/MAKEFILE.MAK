CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  =  -c -g -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj datafile.obj card.obj sound.obj asc2sjis.obj sjis2asc.obj graphic.obj

all  : main.cpe
       echo Done.


main.obj: main.c main.h card.h ctrller.h sound.h
	$(CC) $(CCOPTIONS) main.c -omain.obj

datafile.obj: datafile.asm
 	asmpsx /l datafile.asm,datafile.obj


card.obj: card.c main.h	card.h
	$(CC) $(CCOPTIONS) card.c -ocard.obj

sound.obj: sound.c main.h	sound.h
	$(CC) $(CCOPTIONS) sound.c -osound.obj

asc2sjis.obj: asc2sjis.c main.h	card.h
	$(CC) $(CCOPTIONS) asc2sjis.c -oasc2sjis.obj

sjis2asc.obj: sjis2asc.c main.h	card.h
	$(CC) $(CCOPTIONS) sjis2asc.c -osjis2asc.obj

graphic.obj: graphic.c main.h	graphic.h
	$(CC) $(CCOPTIONS) graphic.c -ographic.obj


main.cpe: $(TEST_OBJS) main.lnk
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map


