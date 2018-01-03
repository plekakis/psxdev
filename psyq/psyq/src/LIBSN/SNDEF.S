;
;	SN Systems default variables
;
; these will be taken from this module if the user does not define
; them as static vars
;

	opt	c+

	section	.rdata
	section	.text
	section	.data
	section	.sdata
	section	.sbss
	section	.bss

	xdef	_stacksize,_ramsize

	section	.data
	cnop	0,4	;longword align

_stacksize	dw	$00008000	;default stack is 32k
_ramsize	dw	$00800000	; 8 Megabytes ** default for MW 3.0

	end

