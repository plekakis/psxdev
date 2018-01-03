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
CCOPTS		= -Wunused -comments-c++ -c -O2 -DFINAL -I.. -I.


# Define to compile with debug.
#CCOPTS		= -Wunused -comments-c++ -c -g -I.. -I. -DDEBUG


ASMOPTS		= /l /c
LINKOPTS	= /m /c /g 

# ----------------------------------------------------------------------------

OBJS		= obj\main.obj obj\movie.obj obj\control.obj

# ----------------------------------------------------------------------------

all: main.cpe
	

main.cpe: $(OBJS) main.lnk makefile 
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map

# ------------------------------------------------------------------------------

obj\main.obj: main.c main.h
	$(CC) $(CCOPTS) main.c -o obj\main.obj


obj\movie.obj: movie.c movie.h
	$(CC) $(CCOPTS) movie.c -o obj\movie.obj


obj\control.obj: control.c control.h
	$(CC) $(CCOPTS) control.c -o obj\control.obj

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
