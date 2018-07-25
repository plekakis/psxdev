;
; $PSLibId: Run-time Library Release 4.4$
;

; assembler file : address.s

	opt	c+

balls	group

	xdef	LoadAddress

	section	.rdata		; define as read-only data.

LoadAddress	dw	group(balls)
; Load address for child process. This is referred in parent(MENU.C).
