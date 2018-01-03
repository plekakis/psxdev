	opt	c+

	section	.text

; int creat(char *name, int perms)
;	    a0         a1

	xdef	PCcreat

PCcreat:
	move	a1,a0
	move	a2,zero
	break	$0102	; fsCREATE
	beq	r2,zero,@ok
	move	r2,r3
	li	r2,-1
@ok:
	jr	ra
	nop

	end

