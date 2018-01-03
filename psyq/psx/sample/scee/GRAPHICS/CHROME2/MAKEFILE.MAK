# ----------------------------------------------------------------------------
# - Psymake Makefile
# ----------------------------------------------------------------------------

ASM		= asmpsx
CC		= ccpsx
LINK		= psylink

# ----------------------------------------------------------------------------

# Add NTSC definition to compile for NTSC (undefined for PAL).
# Add TESTING definition for debug information while playing the movie.
# Add FIND_VLCBUF definition to find maximum runlevel.


# Define to compile final version. If defined removes all pollhosts, set 2MB
# and ramsize. Else use pollhosts and 8MB for development. 
#CCOPTS		= -Wunused -comments-c++ -c -O2 -DFINAL -I.. -I.


# Define to compile with debug.
CCOPTS		= -Wunused -comments-c++ -c -g -I.. -I. -DDEBUG -O2 -mgpopt -G16 -DNTSC


ASMOPTS		= /l /c
LINKOPTS	= /m /c /g 

# ----------------------------------------------------------------------------

OBJS		= obj\main.obj obj\init.obj obj\tmd_shit.obj

# ----------------------------------------------------------------------------

all: main.cpe
	

main.cpe: $(OBJS) main.lnk makefile 
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map

# ------------------------------------------------------------------------------

obj\main.obj: main.c main.h
	$(CC) $(CCOPTS) main.c -o obj\main.obj
	dmpsx -b obj\main.obj 


obj\tmd_shit.obj: tmd_shit.c
	$(CC) $(CCOPTS) tmd_shit.c -o obj\tmd_shit.obj
	dmpsx -b obj\tmd_shit.obj 


obj\init.obj: init.c 
	$(CC) $(CCOPTS) init.c -o obj\init.obj
	dmpsx -b obj\init.obj 

# ----------------------------------------------------------------------------

bk:
	copy *.h	backup
	copy *.c	backup
	copy makefile	backup
	copy *.mak	backup
	copy *.lnk	backup
	copy *.txt	backup
	copy *.cnf	backup
		
# ------------------------------------------------------------------------------

clean:
	del *.exe
	del *.cpe
	del obj\*.obj
	del *.sym

# ------------------------------------------------------------------------------

load:

	pqbload texmods\textile2.tim 80100000
	pqbload texmods\water1.tim   80105000
	pqbload texmods\textile1.tim 80110000
	pqbload texmods\theship.tmd  80130000
# ------------------------------------------------------------------------------
