
#
# Makefile (PsyMake)
# Mike Acton
#

DEBUG = -O2

#
# Main target
#

main.cpe: main.obj \
          cube.obj 
          psylink /m /c /g @main.lnk,main.cpe,main.sym,main.map

#
# Compile dependencies
#

main.obj:      main.c
cube.obj:      cube.c

#
# Implicit rules
#

.c.obj:
  ccpsx -comments-c++ -Wall -c $(DEBUG) $&.c -o $&.obj
  dmpsx $&.obj
.s.o:
  asmpsx /o c+ /l /zd $&.s,$&.o,$&.sym

