# ----------------------------------------------------------------------------
# - Project:    WATCOM Makefile
# -
# - Author:	Vince Diesi
# ----------------------------------------------------------------------------

ASM             = asmpsx
CC              = ccpsx
LINK            = psylink

# ------------------------------------------------------------------------------

OBJDIR		= obj

# ------------------------------------------------------------------------------

# Define to compile final version. If defined removes all pollhosts, set 2MB
# and ramsize. Else use pollhosts and 8MB for development. 
FINAL		=

# Define to compile with None2.lib. Otherwise, libsn.lib is used.
#NONE2		=

# Define for debug information while playing the movie.
TESTING		=

# Define to compile with debug.
#DEBUG		=
#DEBUG2		=
#DEBUG3		=

# Define to compile for NTSC (undefined for PAL).
#NTSC		=

# Define to find maximum runlevel (i.e. max size of each VLC buffer required
# to play the stream).
#FIND_VLCBUF	=

# ------------------------------------------------------------------------------

CCOPTS		= -Wunused -comments-c++ -c -I.. -I.

!ifdef FINAL
CCOPTS		+= -DFINAL
!endif

!ifdef NONE2
CCOPTS		+= -DLINKED_NONE2
!endif

!ifdef TESTING
CCOPTS		+= -DTESTING
!endif

!ifdef DEBUG
CCOPTS		+= -DDEBUG
!endif

!ifdef DEBUG2
CCOPTS		+= -DDEBUG2
!endif

!ifdef DEBUG3
CCOPTS		+= -DDEBUG3
!endif

!ifdef NTSC
CCOPTS		+= -DNTSC
!endif

!ifdef FIND_VLCBUF
CCOPTS		+= -DFIND_VLCBUF
!endif

# Optmise or debug.
#CCOPTS		+= -O3
CCOPTS		+= -O2
#CCOPTS		+= -g

# ------------------------------------------------------------------------------

ASMOPTS         = /l /c

LINKOPTS        = /m /c /g 

# ------------------------------------------------------------------------------

OBJS	=	$(OBJDIR)\main.obj	$(OBJDIR)\movie.obj	&
		$(OBJDIR)\control.obj

# ------------------------------------------------------------------------------

!ifdef FINAL

all: main.exe .SYMBOLIC

!else

all: main.cpe .SYMBOLIC

!endif	
	@ echo Compilation Successful!
	@ echo -----------------------	

!ifdef NONE2
	@ echo 1: Link with None2.lib, not libsn.lib.
!else
	@ echo 1: Link with libsn.lib, not None2.lib.
!endif

!ifdef FINAL
	@ echo 2: Compiled for final machine. EXE also created.
!else
	@ echo 2: Compiled for development.
!endif	

!ifdef NTSC
	@ echo 3: Compiled for NTSC. 
!else
	@ echo 3: Compiled for PAL. 
!endif

!ifdef FIND_VLCBUF
	@ echo 4: Report maximum VLC buffer size.
!endif

# ------------------------------------------------------------------------------

main.exe: main.cpe makefile
	cpe2x /ce main.cpe

# ------------------------------------------------------------------------------

main.cpe: $(OBJS) main.lnk makefile 
	$(LINK) $(LINKOPTS) @main.lnk,main.cpe,main.sym,main.map

# ------------------------------------------------------------------------------

$(OBJDIR)\main.obj: main.c main.h
	$(CC) $(CCOPTS) main.c -o $@


$(OBJDIR)\movie.obj: movie.c movie.h
	$(CC) $(CCOPTS) movie.c -o $@


$(OBJDIR)\control.obj: control.c control.h
	$(CC) $(CCOPTS) control.c -o $@

# ----------------------------------------------------------------------------

bk: .SYMBOLIC
	copy *.h	backup
	copy *.c	backup
	copy makefile	backup
	copy *.mak	backup
	copy *.lnk	backup
	copy *.txt	backup
	copy *.cnf	backup
		
# ------------------------------------------------------------------------------

clean: .SYMBOLIC
	del *.exe
	del *.cpe
	del $(OBJDIR)\*.obj
	del *.sym

# ------------------------------------------------------------------------------
