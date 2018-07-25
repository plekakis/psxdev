COPTIONS =  -g -comments-c++ -v

LINKADDRESS=$80080000

all:
	ccpsx $(COPTIONS) -Xo$(LINKADDRESS) hello.cc -ohello.cpe,hello.sym
