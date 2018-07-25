SRCS =	balls.c diffuse.c rgb24.c mat.c morph.c mattex.c glasstex.c
OBJS =	balls.obj diffuse.obj rgb24.obj mat.obj morph.obj \
	mattex.obj glasstex.obj
main :
	ccpsx -O3 -c $(SRCS)
	ccpsx -O3 -Xo$80020000 main.c $(OBJS) -omain.cpe,main.sym

for_menu:
	ccpsx -O3 -Xo$80020000 main.c $(SRCS) \psx\lib\none3.obj -omain.cpe
	cpe2x main.cpe
	del main.cpe
