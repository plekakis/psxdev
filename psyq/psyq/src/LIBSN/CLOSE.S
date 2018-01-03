	opt	c+

	section	.text

; int close(int fd)
;               a0

	xdef	PCclose

PCclose:
	move	a1,a0
	break	$0104	; fsCLOSE
	jr	ra
	nop

	end

