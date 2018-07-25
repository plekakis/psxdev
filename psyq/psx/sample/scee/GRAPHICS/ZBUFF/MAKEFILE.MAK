CC   = ccpsx
ASM  = asmpsx
LINK = psylink

CCOPTIONS  =  -c -W -O2 -comments-c++
ASMOPTIONS = /l /o c+,h+,at- /zd
LINKOPTS   = -Xo$80028000 -Xm -Xn999
TEST_OBJS  = main.obj zbf.obj datafile.obj

all  : main.cpe
       echo Done.

zbf.obj: zbf.asm
	$(ASM) /l /j c:\psx\include zbf.asm,zbf.obj
	dmpsx zbf.obj

datafile.obj: datafile.asm
        $(ASM) /l datafile.asm,datafile.obj

main.obj: main.c
	$(CC) $(CCOPTIONS) main.c -omain.obj
	dmpsx main.obj

main.cpe: $(TEST_OBJS)
        ccpsx $(TEST_OBJS) $(LINKOPTS) -omain.cpe,main.sym,main.map

