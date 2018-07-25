'ASM  = asmpsx
CC   = ccpsx
LINK = psylink

CCOPTIONS  =  -c -O2  -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj graph.obj tmd.obj effects.obj datafile.obj 


all  : main.cpe
       echo Done.

main.obj: main.c main.h
        $(CC) $(CCOPTIONS) main.c -omain.obj
		dmpsx main.obj


graph.obj: graph.c main.h
	$(CC) $(CCOPTIONS) graph.c  -ograph.obj
	dmpsx graph.obj

tmd.obj: tmd.c main.h tmd.h ctrller.h
	$(CC) $(CCOPTIONS) tmd.c  -otmd.obj
	dmpsx tmd.obj

effects.obj: effects.c effects.h main.h
	$(CC) $(CCOPTIONS) effects.c  -oeffects.obj
	dmpsx tmd.obj

datafile.obj: datafile.asm
        asmpsx /l datafile.asm,datafile.obj

main.cpe: $(TEST_OBJS) main.lnk
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map
clean:
        del *.obj
        del *.sym
        del *.cpe
        del *.map


