
	opt	c+

text	group				; main text group
bss	group	bss			; main data group
spad	group	obj($1f800000)

	xdef	SpadStock

	section	.rdata

SpadStock	dw	grouporg(spad)
; grouporg()はグループのORGアドレスを返す特殊関数。
; 「DTL-D2130 Psy-Q Programmer's Guide」P.93 参照。
