OBJS = none2.obj 

none2.lib: $(OBJS)
        del none2.lib
        psylib /a none2.lib *.obj

none2.obj: none2.s
        asmpsx /l none2,none2




