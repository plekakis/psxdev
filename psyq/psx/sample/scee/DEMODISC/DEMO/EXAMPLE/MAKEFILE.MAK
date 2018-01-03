ASM             = asmpsx
CC              = ccpsx
LINK            = psylink

CHILDOPTS          = -c -O2 -DLINKED_NONE2

ASMOPTS         = /l /c

LINKOPTS        = /m /c /g 

APP_ORG         = -Xo$80018000

CDOPTIONS       =  -s0:2 

#------------------------------------------------------------------------------

cd: example.cti harness.exe bs.exe
        buildcd $(CDOPTIONS) example.cti

#------------------------------------------------------------------------------

harness.exe: harness.cpe
        cpe2x harness.cpe
        setsp harness.exe 801f0000

harness.obj: harness.c 
	$(CC) $(CHILDOPTS) $(APP_ORG) -oharness.obj harness.c

harness.cpe: harness.obj 
	$(LINK) $(LINKOPTS) @harness.lnk,harness.cpe,harness.sym,harness.map

#------------------------------------------------------------------------------
