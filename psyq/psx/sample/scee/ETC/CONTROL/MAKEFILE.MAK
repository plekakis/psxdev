CC = ccpsx
OPTIONS = -g -Xo$80010000 -comments-c++
OPT2 = -O2 -Xo$80010000

  
control.cpe: control.c
        $(CC) $(OPTIONS) control.c -ocontrol.cpe,contol.sym,control.map -l libtap.lib

