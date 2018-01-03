;
; $PSLibId: Run-time Library Release 4.4$
;


	opt	c+

text	group				; main text group
bss	group	bss			; main data group
spad	group	obj($1f800000)

	xdef	SpadStock

	section	.rdata

SpadStock	dw	grouporg(spad)
; grouporg() returns the ORG address of the group.
