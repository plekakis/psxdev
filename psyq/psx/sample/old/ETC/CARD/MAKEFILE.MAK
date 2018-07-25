all:
	ccpsx -O -Xo$80010000 card.c -ocard.cpe,card.sym


for_menu:
	ccpsx -O -Xo$80010000 card.c \psx\lib\none3.obj -ocard.cpe,card.sym
	cpe2x card.cpe
	del card.cpe

