CC = ccpsx
ASM = asmpsx
LINK = psylink

#CCOPTIONS =  -c -O2 -I. -DDEX2000
CCOPTIONS =  -c -g -I. -DDEBUG -DDEX2000

ASMOPTIONS = /l /o c+,h+,at- /zd

LINKOPTS = /m

TEST_OBJS = profile.obj protest.obj

all : protest.cpe
        echo Done.

tidy :
        del *.obj
        del *.cpe
        touch *.c
        touch *.s

clean: tidy all
        echo Done.

profile.obj: profile.c profile.h makefile.mak
	$(CC) $(CCOPTIONS) profile.c -oprofile.obj

protest.obj: protest.c makefile.mak
	$(CC) $(CCOPTIONS) protest.c -oprotest.obj

# Old way: 
#protest.cpe: $(TEST_OBJS) 
#        $(LINK) $(LINKOPTS) @protest.lnk,protest.cpe,protest.sym,protest.map

# Another way (Chia-Ming Wang)
protest.cpe : $(TEST_OBJS)
        $(CC) -Xo$80010000 -o protest.cpe $(TEST_OBJS)

