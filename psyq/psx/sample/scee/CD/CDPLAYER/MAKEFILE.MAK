#
# Dave 18/12/95
#
all: cddademo
	
cddademo:

	ccpsx -O -Xo$80010000 cddademo.c -ocddademo.cpe,cddademo.sym 
