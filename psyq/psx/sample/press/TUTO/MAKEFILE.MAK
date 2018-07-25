SRCS  = tuto1.c tuto2.c tuto3.c tuto4.c tuto5.c tuto6.c tuto7.c \
	buffer.c mdec_bs.c
OBJS  = tuto1.obj tuto2.obj tuto3.obj tuto4.obj tuto5.obj \
	tuto6.obj tuto7.obj buffer.obj mdec_bs.obj
main: tuto.lib
	ccpsx -O3 -Xo$80020000 main.c -ltuto.lib -omain.cpe,main.sym

for_menu: tuto.lib
	ccpsx -O3 -Xo$80020000 main.c \psx\lib\none3.obj -ltuto.lib -omain.cpe
	cpe2x main.cpe
	del main.cpe

tuto.lib:
	ccpsx -O3 -c $(SRCS) 
	del tuto.lib
	psylib /a tuto.lib $(OBJS)

clean:
	del *.lib
	del *.obj
	del *.cpe
	del *.sym
	del *.exe
