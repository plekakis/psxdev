	opt	c+

	section	.text

; int _SN_read (int op, int fd, int n, char *buf)
;	  a0        a1       a2      a3

	xdef	_SN_read

_SN_read:
	break	$0105	; fsREAD
	beq	r2,zero,@ok
	move	r2,r3
	li	r2,-1
@ok:
	jr	ra
	nop

	end

