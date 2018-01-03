	opt	c+

	section	.text

; long lseek(int fd, long offset, int origin)
;	   a0        a1          a2

	xdef	PClseek

PClseek:
	move	a3,a2
	move	a2,a1
	move	a1,a0
	break	$0107	; fsSEEK
	beq	r2,zero,@ok
	move	r2,r3
	li	r2,-1
@ok:
	jr	ra
	nop

	end

