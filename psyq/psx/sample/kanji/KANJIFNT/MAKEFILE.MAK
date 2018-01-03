#
# file : makefile.mak
#
#	makefile for psymake
#
#
main.cpe: main.obj trans.obj kanji.h
	ccpsx -O2 -Xo$80080000 main.obj trans.obj -omain.cpe,main.sym

for_menu: main.obj trans.obj kanji.h
	ccpsx -O2 -Xo$80080000 main.obj trans.obj \psx\lib\none3.obj -omain.cpe
	cpe2x main.cpe
	del main.cpe

main.obj: main.c kanji.h
	ccpsx -c -O2 -kanji main.c

trans.obj: trans.c kanji.h
	ccpsx -c -O2 trans.c

clean:
	del *.obj
	del main.cpe
	del main.sym
	del main.exe
