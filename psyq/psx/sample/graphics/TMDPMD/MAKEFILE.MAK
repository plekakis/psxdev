DATDIR=..\..\..\data
SRCS  =	tuto0.c tuto1.c tuto2.c tuto3.c tuto4.c tuto5.c tuto6.c db.c 
OBJS  =	tuto0.obj tuto1.obj tuto2.obj tuto3.obj tuto4.obj tuto5.obj \
	tuto6.obj db.obj 
main :
	ccpsx -O3 -c $(SRCS)
	dmpsx tuto0.obj
	dmpsx tuto1.obj
	dmpsx tuto6.obj
	ccpsx -O3 -Xo$80020000 main.c $(OBJS) -omain.cpe,main.sym
load :
	pqbload $(DATDIR)\pmd\share.pmd   800d0000
	pqbload $(DATDIR)\pmd\noshare.pmd 800e0000
	pqbload $(DATDIR)\tim\pat.tim     800f0000
	pqbload $(DATDIR)\tmd\dino.tmd    80100000
	pqbload $(DATDIR)\tmd\earth.tmd   80120000
	pqbload $(DATDIR)\tim\earth4.tim  80140000

for_menu: $(OBJS)
	ccpsx -O3 -c main.c 
	psylink /c /m @menu.lnk,tmdpmd.cpe
	cpe2x tmdpmd.cpe
	del tmdpmd.cpe

db.obj : db.c
	ccpsx -O3 -c db.c

tuto0.obj : tuto0.c
	ccpsx -O3 -c tuto0.c
	dmpsx tuto0.obj

tuto1.obj : tuto1.c
	ccpsx -O3 -c tuto1.c
	dmpsx tuto1.obj

tuto2.obj : tuto2.c
	ccpsx -O3 -c tuto2.c

tuto3.obj : tuto3.c
	ccpsx -O3 -c tuto3.c

tuto4.obj : tuto4.c
	ccpsx -O3 -c tuto4.c

tuto5.obj : tuto5.c
	ccpsx -O3 -c tuto5.c

tuto6.obj : tuto6.c
	ccpsx -O3 -c tuto6.c
	dmpsx tuto6.obj

clean:
	del *.obj
	del *.cpe
	del *.sym
	del *.exe
