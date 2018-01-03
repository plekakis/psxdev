DEFINES	=
CFLAGS	= -O3

LIB	= common.lib
OBJS	= poly.obj shared.obj image.obj anim.obj mime.obj \
	  gnd.obj envmap.obj equip.obj line.obj norm.obj pad.obj
NOBJS	= f0000008.o1 f0000009.o1 f000000c.o1 f000000d.o1 f0000010.o1 \
	  f0000011.o1 f0000014.o1 f0000015.o1

common.lib: $(OBJS) $(NOBJS)
	psylib /u $. @list

.c.obj:
	ccpsx $(CFLAGS) -c -o$. $&.c

.c.o1:
	ccpsx $(CFLAGS) -c -o$&.obj $&.c
	dmpsx $&.obj -o $.
