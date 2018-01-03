CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  =  -c -g -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj card.obj graph.obj cardman.obj sjis2asc.obj text.obj copy.obj inter.obj mainmenu.obj vsex.obj vsdie.obj stream.obj sound.obj

all  : main.cpe
       echo Done.

main.obj: main.c main.h
	$(CC) $(CCOPTIONS) main.c -omain.obj

card.obj: card.c main.h
	$(CC) $(CCOPTIONS) card.c -ocard.obj

cardman.obj: cardman.c main.h
	$(CC) $(CCOPTIONS) cardman.c -ocardman.obj


graph.obj: graph.c main.h
	$(CC) $(CCOPTIONS) graph.c -ograph.obj

sjis2asc.obj: sjis2asc.c main.h
	$(CC) $(CCOPTIONS) sjis2asc.c -osjis2asc.obj

text.obj: text.c main.h
	$(CC) $(CCOPTIONS) text.c -otext.obj

copy.obj: copy.c main.h
	$(CC) $(CCOPTIONS) copy.c -ocopy.obj

inter.obj: inter.c main.h
	$(CC) $(CCOPTIONS) inter.c -ointer.obj

mainmenu.obj: mainmenu.c main.h
	$(CC) $(CCOPTIONS) mainmenu.c -omainmenu.obj

vsex.obj : vsex.s
	asmpsx /l /g vsex.s, vsex.obj


vsdie.obj: vsdie.c
	$(CC) $(CCOPTIONS) vsdie.c  -ovsdie.obj

stream.obj: stream.c
	$(CC) $(CCOPTIONS) stream.c  -ostream.obj

sound.obj: sound.c
	$(CC) $(CCOPTIONS) sound.c  -osound.obj



main.cpe: $(TEST_OBJS) main.lnk
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map


load:
	pqbload backdrop.tim 80180000
	pqbload alpha.tim 80150000
	pqbload vision.tim 80120000

cd:
	buildcd -mcdupdte.map main.cti -s0:1

ucd:
	updatecd cdupdte.map 


map:
	rp
	delay
	run c:\screen\main



bkup2:
       copy *.c  *.e



exe:
       cpe2x /ce main.cpe
       copy main.exe SCES_003.90 
