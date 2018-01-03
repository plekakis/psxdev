#ASM  = asmpsx
CC   = ccpsx
LINK = psylink


CCOPTIONS  =  -c -O2 -mgpopt -G32 -comments-c++ 

# If you want to use NTSC mode, uncomment the following line:
#CCOPTIONS  =  -c -O2 -mgpopt -G32 -comments-c++ -DUSE_NTSC


ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = /m
TEST_OBJS  = main.obj graph.obj

all  : main.cpe
       echo Done.

main.obj: flame.c flame.h graph.h
	$(CC) $(CCOPTIONS) flame.c -omain.obj

graph.obj: graph.c graph.h
        $(CC) $(CCOPTIONS) graph.c -ograph.obj

#main.cpe: $(TEST_OBJS) main.lnk
#	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map


main.cpe : $(TEST_OBJS)
	ccpsx -Xo$80010000 -omain.cpe $(TEST_OBJS)



