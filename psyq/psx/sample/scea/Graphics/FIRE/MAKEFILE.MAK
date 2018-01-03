CC         = ccpsx
LINK       = psylink
COPTS      = -Wall -comments-c++ -O3 -mgpopt -G16
DOPTS  = -g -Wall -comments-c++ 
LINKOPTS   = /m

#------------------------------------------------------------------------------

main.cpe: main.c
        $(CC) $(COPTS) -Xo$80170000 main.c -omain.cpe,main.sym,main.map


clean:
	del *.obj
        del *.exe
        del *.sym
        del *.map
        del *.cpe

run:
	resetps 1
        delay 20
        pqbload .\clut.tim       800140000  # size 0x31B0
	delay 20
        run main.cpe


#------------------------------------------------------------------------------

load:
   pqbload clut.tim      800140000  # size 0x31B0










					   



