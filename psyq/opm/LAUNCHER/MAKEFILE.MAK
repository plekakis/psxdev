ASM             = asmpsx
CC              = ccpsx
LINK            = psylink

COPTS           = -c -O3 -Wall -comments-c++
ASMOPTS         = /l /c
LINKOPTS        = /m /c

MENU_ORG        = -Xo$80010000

CDOPTIONS       =  -s3:1 -mcd.map

#------------------------------------------------------------------------------

              
launch.exe: launch.cpe
        cpe2x /CA launch.cpe

launch.cpe: launch.obj makefile.mak
	$(LINK) $(LINKOPTS) @launch.lnk,launch.cpe,launch.sym,launch.map


#------------------------------------------------------------------------------

cdi: launch.cti 
        buildcd $(CDOPTIONS) launch.cti

#------------------------------------------------------------------------------

launch.obj: launch.c launch.h makefile.mak
	$(CC) $(COPTS) $(MENU_ORG) launch.c -olaunch.obj

#------------------------------------------------------------------------------



