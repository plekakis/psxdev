	opt	c+

	section	.text

; int init()

	xdef	PCinit

PCinit:
	break	$0101	; fsINIT
	jr	ra
	nop

	end

