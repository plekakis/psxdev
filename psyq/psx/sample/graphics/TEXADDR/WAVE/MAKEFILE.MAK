DATDIR=..\..\..\..\data

all: tuto1.cpe tuto2.cpe tuto3.cpe main.cpe
	
for_menu: a_tuto1.exe a_tuto2.exe a_tuto3.exe wave.exe
	
CC_CPE = ccpsx -O3 -G 0 -Xo$80100000 -Xm
CC_EXE = ccpsx -O3 -G 0 -Xo$80100000 -Xm \psx\lib\none3.obj
OBJ    = render.ob2

render.ob2: render.c
	$(CC_CPE) -c render.c
	dmpsx render.obj -o render.ob2

tuto1.ob2: tuto.c
	$(CC_CPE) -DTUNE=1 -c -o tuto1.obj tuto.c
	dmpsx tuto1.obj -o tuto1.ob2

tuto2.ob2: tuto.c
	$(CC_CPE) -DTUNE=2 -c -o tuto2.obj tuto.c
	dmpsx tuto2.obj -o tuto2.ob2

tuto3.ob2: tuto.c
	$(CC_CPE) -DTUNE=3 -c -o tuto3.obj tuto.c
	dmpsx tuto3.obj -o tuto3.ob2

main.ob2: main.c $(OBJ)
	$(CC_CPE) -DTUNE=3 -c -o main.obj main.c
	dmpsx main.obj -o main.ob2

tuto1.cpe: tuto1.ob2 $(OBJ)
	$(CC_CPE) tuto1.ob2 $(OBJ) -otuto1.cpe,tuto1.sym,tuto1.map

tuto2.cpe: tuto2.ob2 $(OBJ)
	$(CC_CPE) tuto2.ob2 $(OBJ) -otuto2.cpe,tuto2.sym,tuto2.map

tuto3.cpe: tuto3.ob2 $(OBJ)
	$(CC_CPE) tuto3.ob2 $(OBJ) -otuto3.cpe,tuto3.sym,tuto3.map

main.cpe: main.c $(OBJ)
	$(CC_CPE) -DTUNE=3 -c -o main.obj main.c
	dmpsx -b main.obj
	$(CC_CPE) main.obj $(OBJ) -omain.cpe,main.sym,main.map

a_tuto1.exe: tuto1.ob2 $(OBJ)
	$(CC_EXE) tuto1.ob2 $(OBJ) -oa_tuto1.cpe
	cpe2x a_tuto1.cpe

a_tuto2.exe: tuto2.ob2 $(OBJ)
	$(CC_EXE) tuto2.ob2 $(OBJ) -oa_tuto2.cpe
	cpe2x a_tuto2.cpe

a_tuto3.exe: tuto3.ob2 $(OBJ)
	$(CC_EXE) tuto3.ob2 $(OBJ) -oa_tuto3.cpe
	cpe2x a_tuto3.cpe

wave.exe: main.ob2 $(OBJ)
	$(CC_EXE) main.ob2 $(OBJ) -owave.cpe
	cpe2x wave.cpe

# tuto1 tuto2 tuto3
load1: 
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave15.tmd 800D0000

# main
load2: 
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave14.tmd 800D0000

tuto1: 
	resetps 1
	delay 20
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave15.tmd 800D0000
	delay 20
	run tuto1

tuto2: 
	resetps 1
	delay 20
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave15.tmd 800D0000
	delay 20
	run tuto2

tuto3: 
	resetps 1
	delay 20
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave15.tmd 800D0000
	delay 20
	run tuto3

main: 
	resetps 1
	delay 20
	pqbload $(DATDIR)\tim\sky.tim     80018000
	pqbload $(DATDIR)\tim\tile.tim    80068000
	pqbload $(DATDIR)\tim\black.tim   80090000
	pqbload $(DATDIR)\tmd\clock.tmd   800C8000
	pqbload $(DATDIR)\tmd\gwave14.tmd 800D0000
	delay 20
	run main
