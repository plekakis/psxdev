ASM             = asmpsx
CC              = ccpsx
LINK            = psylink

DDPOPTS          = -c -g -DDEBUG -I.. -DBOOTTIMEOUT=250
#DDPOPTS          = -c -O2 -I.. -DBOOTTIMEOUT=250 

ASMOPTS         = /l /c

LINKOPTS        = /m /c /g 

MENU_ORG        = -Xo$80010000

CDOPTIONS       =  -s0:2

#------------------------------------------------------------------------------

              
bs.exe: bs.cpe
        cpe2x bs.cpe

bs.cpe: bs.obj icon.obj makefile.mak
	$(LINK) $(LINKOPTS) @bs.lnk,bs.cpe,bs.sym,bs.map


#------------------------------------------------------------------------------

cdi: bs.cti 
        buildcd $(CDOPTIONS) bs.cti

#------------------------------------------------------------------------------

bs.lib:
        cd lib
        del bs.lib
        psylib /a bs.lib *.obj
        cd ..

#------------------------------------------------------------------------------

bs.obj: bs.c ddp.h makefile.mak
	$(CC) $(DDPOPTS) $(MENU_ORG) bs.c -obs.obj

icon.obj : icon.tim icon.asm
	$(ASM) $(ASMOPTS) icon.asm,icon.obj

#------------------------------------------------------------------------------
