	opt	c+

	section	.text

; int open(char *name, int flags, int perms)
;	  a0         a1         a2

	xdef	PCopen

PCopen:
	move	a2,a1
	move	a1,a0
	break	$0103	; fsOPEN
	beq	r2,zero,@ok
	move	r2,r3
	li	r2,-1
@ok:
	jr	ra
	nop

	end

