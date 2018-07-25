OBJS = startup.obj 

startup.obj: startup.s
        asmpsx /l startup,startup
