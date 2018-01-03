	opt	c+

	section	.text

; void pollhost()

	xdef	pollhost

pollhost:
	break	$0001	; POLHOST
	jr	ra
	nop

	end




